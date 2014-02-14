#include "SRASConnection.h"
#include "websocket.h"

#define BUFFER_SIZE 0xFFFF

void wsMakeFrame(const std::string data, ByteBuffer *outFrame, enum wsFrameType frameType)
{
    outFrame->WriteBit(1);
    outFrame->WriteBits(0, 3);
    outFrame->WriteBits(frameType, 4); //Opcode text
    outFrame->WriteBit(0); //No mask

    int size = data.size();
    if(size <= 125)
        outFrame->WriteBits(size, 7);
    else if(size <= 0xFFFF)
    {
        outFrame->WriteBits(126, 7);
        outFrame->WriteBits(size, 16);
    }
    else
    {
        outFrame->WriteBits(127, 7);
        outFrame->WriteBits(size, 64);
    }

    outFrame->FlushBits();

    outFrame->WriteString(data);
}

SRASConnection::SRASConnection(int socket) : m_socket(socket)
{
    m_ws_frameType = WS_INCOMPLETE_FRAME;
    m_ws_state = WS_STATE_OPENING;

    m_currentReceivedFrame.clear();
    m_currentSendFrame.clear();
}

SRASConnection::~SRASConnection()
{
    sLog->outDebug(LOG_FILTER_SRAS, "Closing a connection");

    shutdown(m_socket, SHUT_RDWR);

    /*if(close(m_socket) < 0)
        sLog->outDebug(LOG_FILTER_SRAS, "Erreur closing socket");*/
}

int SRASConnection::ReadyRead()
{
    uint8 buffer[BUFFER_SIZE];
    handshake hs;
    nullHandshake(&hs);

    uint8 *frameBuffer = NULL;
    uint64 dataSize = 0;

    int received = recv(m_socket, buffer, BUFFER_SIZE-1, 0);

    if(received == -1)
    {
        if(errno == EAGAIN || errno == EWOULDBLOCK)
            return 0;

        sLog->outDebug(LOG_FILTER_SRAS, "SRAS reading error");
        return -1;
    }

    if(received == 0)
        return -1;

    if(received > 0)
        m_currentReceivedFrame.append(buffer, received);

    //buffer[received] = 0;

    //sLog->outInfo(LOG_FILTER_SRAS, "SRAS Data : %s", (char*)buffer);

    if(m_ws_state == WS_STATE_OPENING)
    {
        m_ws_frameType = wsParseHandshake(m_currentReceivedFrame.contents(), m_currentReceivedFrame.size(), &hs);

    }
    else
    {
        m_ws_frameType = wsParseInputFrame(m_currentReceivedFrame.contents(), m_currentReceivedFrame.size(), &frameBuffer, &dataSize);
    }

    if(m_ws_frameType == WS_INCOMPLETE_FRAME)
        return 0;

    if(m_ws_frameType == WS_ERROR_FRAME)
    {
        if(m_ws_state == WS_STATE_OPENING)
        {
            sLog->outDebug(LOG_FILTER_SRAS, "Bad handshake request (dropping)");
            ByteBuffer buff;
            buff << "HTTP/1.1 400 Bad Request\r\n" << versionField << version << "\r\n";
            send(m_socket, buff.contents(), buff.size(), 0);
        }
        else
            sLog->outDebug(LOG_FILTER_SRAS, "Frame error");

        return -1;
    }

    if(m_ws_frameType == WS_EMPTY_FRAME)
    {
        m_currentReceivedFrame.clear();
        return 0;
    }

    if(m_ws_state == WS_STATE_NORMAL)
    {
        if(dataSize == 0)
            return -1;

        ByteBuffer frame;
        frame.append(frameBuffer, dataSize);
        std::string packet = frame.ReadString(dataSize);
        sLog->outDebug(LOG_FILTER_SRAS, "New frame %s", packet.c_str());

        try
        {
            SRASPacket pkt(packet);
            pkt.next();
            int opcode = pkt.toInt();

            if(opcode == 0)
                throw std::string("Null opcode");

            m_currentReceivedFrame.clear();

            return HandlePacket(opcode, pkt);
        }
        catch (std::string msg)
        {
            sLog->outError(LOG_FILTER_SRAS, "[SRAS Exception] %s", msg.c_str());
        }
    }

    if(m_ws_state == WS_STATE_OPENING)
    {
        if(m_ws_frameType == WS_OPENING_FRAME)
        {
            uint8 outBuffer[BUFFER_SIZE];
            uint64 outLenght;
            wsGetHandshakeAnswer(&hs, (uint8*)&outBuffer, &outLenght);

            send(m_socket, outBuffer, outLenght, 0);

            m_ws_state = WS_STATE_NORMAL;
        }
    }

    if(m_ws_frameType == WS_CLOSING_FRAME)
    {
        if(m_ws_state == WS_STATE_CLOSING)
        {
            sLog->outDebug(LOG_FILTER_SRAS, "Closing the connection");
            return -1; //Acquittement du client
        }
        else
        {
            /*uint8 outBuffer[BUFFER_SIZE];
            uint64 outLenght;
            uint16 error = 1000;
            wsMakeFrame((uint8*)&error, sizeof(uint16), (uint8*)&outBuffer, &outLenght, WS_CLOSING_FRAME);

            send(m_socket, outBuffer, outLenght, 0);

            m_ws_state = WS_STATE_CLOSING;*/

            return -1;
        }
    }

    m_currentReceivedFrame.clear();

    return 0;
}

int SRASConnection::ReadySend()
{
    if(m_currentSendFrame.size() == 0) //Est-on pret a envoyer un nouveau message
    {
        if(m_queuedMessage.empty())
            return 0;

        std::string newFrameData = m_queuedMessage.front();

        uint8 *frameBuffer = new uint8[BUFFER_SIZE];
        uint64 frameSize = 0;

        sLog->outDebug(LOG_FILTER_SRAS, "Sending frame : %s", newFrameData.c_str());

        wsMakeFrame(newFrameData, &m_currentSendFrame, WS_TEXT_FRAME);

        send(m_socket, m_currentSendFrame.contents(), m_currentSendFrame.size(), 0); //On l'envoi

        delete[] frameBuffer;
        m_currentSendFrame.clear();

        m_queuedMessage.pop();
    }

    /*int sentByte = send(m_socket, m_currentSendFrame.contents(), m_currentSendFrame.size(), 0); //On l'envoi

    if(sentByte == -1) //S'il y a une erreur...
    {
        if(errno == EAGAIN || errno == EWOULDBLOCK) //...et que l'erreur est est un signal de blocage qui indique que la socket est occupe...
            return 0; //...on retourne sans rien dire et on retente au prochain coup...
        else //...sinon...
            return -1; //...on renvoie une erreur
    }

    if(sentByte >= m_currentSendFrame.size()) //Si le message a ete correctement envoye, alors on est pret a en envoyer un autre
    {
        m_currentSendFrame.clear();
    }
    else if(sentByte < m_currentSendFrame.size()) //Si le message a ete partiellement envoyé
    {
        uint8 frameBuffer[BUFFER_SIZE];
        uint32 bufferSize = m_currentSendFrame.size()-sentByte;
        memcpy(&frameBuffer, m_currentSendFrame.contents()+sentByte, bufferSize); //On efface ce qui a ete envoye et on renvoie le reste au prochain coup
        m_currentSendFrame.clear();
        m_currentSendFrame.append(&frameBuffer, bufferSize);
    }*/

    return 0;
}

void SRASConnection::SendPacket(std::string buffer)
{
    m_queuedMessage.push(buffer);
}

int SRASConnection::HandlePacket(int opcode, SRASPacket pkt)
{
    switch(opcode)
    {
        case AUTH_CHALLENGE:
            AuthChallenge(pkt);
            break;
        case TICKET_LIST:
            TicketList();
            break;
        case SEARCH_QUERY:
            SearchQuery(pkt);
            break;
        case SERVER_ANNOUNCE:
            ServerAnnounce(pkt);
            break;
        default:
            char msg[256];
            sprintf(msg, "Unknow opcode %u", opcode);
            throw std::string(msg);
    }

    return 0;
}

template void SRASPacket::add(uint8);
template void SRASPacket::add(uint16);
template void SRASPacket::add(uint32);
template void SRASPacket::add(uint64);
template void SRASPacket::add(int8);
template void SRASPacket::add(int16);
template void SRASPacket::add(int32);
template void SRASPacket::add(int64);
