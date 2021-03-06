/*
 * Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */


#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "Player.h"
#include "SpellInfo.h"
#include "SpellScript.h"
#include "GridNotifiers.h"
#include "ScriptedEscortAI.h"
#include "Vehicle.h"
#include "CombatAI.h"

/*******************************/
/********* AreaTrigger *********/
/*******************************/

enum AreaTriggerPool
{
    SPELL_FROG_CURSE    = 102938
};

typedef std::map<Player*, uint32> InsideAreaTriggerCheckMap ;

class at_meditation_pool : public AreaTriggerScript
{
public :
    at_meditation_pool() : AreaTriggerScript("at_meditation_pool")
    {
        check.clear();
    }

    bool OnTrigger(Player *player, const AreaTriggerEntry *areaTrigger)
    {
        /*
         * First, check if the player has been or not registered in the map
         * If not add it, otherwise, do some checks
         */
        InsideAreaTriggerCheckMap::iterator iter = check.find(player);
        if(iter == check.end())
        {
            /*
             * Player is not yet registered in the checker
             * We must add it
             */
            check[player] = areaTrigger->id ;
            player->CastSpell(player, SPELL_FROG_CURSE, true);
        }
        else
        {
            /*
             * We have many cases to handle here : did the player exited the actual area ?
             * Did it triggered the other, while inside the actual ?
             */

            // Player leaved the areaTrigger
            if(areaTrigger->id == iter->second)
            {
                check.erase(iter);
                player->RemoveAurasDueToSpell(SPELL_FROG_CURSE);
            }
            // Player switched AreaTrigger
            else
            {
                 iter->second = areaTrigger->id ;
            }
        }
        return true;
    }

private :
    InsideAreaTriggerCheckMap check ;
};

enum Area7736
{
    NPC_HUOJIN_MONK             = 60176,
    QUEST_THE_SPIRIT_GUARDIAN   = 29420,
    SAY_HUOJIN                  = 0
};

class at_huojin_monk_talk : public AreaTriggerScript
{
public :
    at_huojin_monk_talk() : AreaTriggerScript("at_huojin_monk_talk") {}

    bool OnTrigger(Player *player, const AreaTriggerEntry *at)
    {
        std::map<uint64, uint32>::iterator iter = forbiddenPlayers.find(player->GetGUID());
        if(iter != forbiddenPlayers.end())
            return false;
        if(player->GetQuestStatus(QUEST_THE_SPIRIT_GUARDIAN) == QUEST_STATUS_COMPLETE)
        {
            Creature* huojin = player->FindNearestCreature(NPC_HUOJIN_MONK, 500.0f);
            if(huojin)
            {
                forbiddenPlayers.insert(std::pair<uint64, uint32>(player->GetGUID(), 120000));
                huojin->AI()->Talk(SAY_HUOJIN, player->GetGUID());
                return true;
            }
        }
        return false;
    }

    void Update(const uint32 uiDiff)
    {
        for (std::map<uint64, uint32>::iterator iter = forbiddenPlayers.begin() ; iter != forbiddenPlayers.end() ; ++iter)
        {
            if(iter->second <= uiDiff)
                forbiddenPlayers.erase(iter);
            else
                iter->second -= uiDiff ;
        }
    }

private :
    std::map<uint64, uint32> forbiddenPlayers ;
};


enum Area7737
{
    NPC_JAOMIN_RO                   = 54611,
    QUEST_THE_DISCIPLE_CHALLENGE    = 29409,
    SAY_JAOMIN                      = 0
};

class at_jaomin_ro_talk : public AreaTriggerScript
{
public :
    at_jaomin_ro_talk() : AreaTriggerScript("at_jaomin_ro_talk") {}

    bool OnTrigger(Player *player, const AreaTriggerEntry *at)
    {
        std::map<uint64, uint32>::iterator iter = forbiddenPlayers.find(player->GetGUID());
        if(iter != forbiddenPlayers.end())
            return false;
        if(player->GetQuestStatus(QUEST_THE_DISCIPLE_CHALLENGE) == QUEST_STATUS_INCOMPLETE)
        {
            Creature* jaomin = player->FindNearestCreature(NPC_JAOMIN_RO, 500.0f);
            if(jaomin)
            {
                forbiddenPlayers.insert(std::pair<uint64, uint32>(player->GetGUID(), 120000));
                jaomin->AI()->Talk(SAY_JAOMIN, player->GetGUID());
                return true;
            }
        }
        return false;
    }

    void Update(const uint32 uiDiff)
    {
        for (std::map<uint64, uint32>::iterator iter = forbiddenPlayers.begin() ; iter != forbiddenPlayers.end() ; ++iter)
        {
            if(iter->second <= uiDiff)
                forbiddenPlayers.erase(iter);
            else
                iter->second -= uiDiff ;
        }
    }

private :
    std::map<uint64, uint32> forbiddenPlayers ;
};


enum Area7746
{
    NPC_TRAINEE_NIM = 60183,
    SAY_NIM         = 0
};

class at_trainee_nim_talk : public AreaTriggerScript
{
public :
    at_trainee_nim_talk() : AreaTriggerScript("at_trainee_nim_talk") {}

    bool OnTrigger(Player *player, const AreaTriggerEntry *at)
    {
        std::map<uint64, uint32>::iterator iter = forbiddenPlayers.find(player->GetGUID());
        if(iter != forbiddenPlayers.end())
            return false;
        if(player->GetQuestStatus(QUEST_THE_DISCIPLE_CHALLENGE) == QUEST_STATUS_INCOMPLETE)
        {
            Creature* nim = player->FindNearestCreature(NPC_TRAINEE_NIM, 500.0f);
            if(nim)
            {
                forbiddenPlayers.insert(std::pair<uint64, uint32>(player->GetGUID(), 120000));
                nim->AI()->Talk(SAY_NIM, player->GetGUID());
                return true;
            }
        }
        return false;
    }

    void Update(const uint32 uiDiff)
    {
        for (std::map<uint64, uint32>::iterator iter = forbiddenPlayers.begin() ; iter != forbiddenPlayers.end() ; ++iter)
        {
            if(iter->second <= uiDiff)
                forbiddenPlayers.erase(iter);
            else
                iter->second -= uiDiff ;
        }
    }

private :
    std::map<uint64, uint32> forbiddenPlayers ;
};

enum Area7747
{
    NPC_TRAINEE_GUANG           = 60244,
    SAY_GUANG                   = 0,
    QUEST_AYSA_OF_THE_TUSHUI    = 29410
};

class at_trainee_guang_talk : public AreaTriggerScript
{
public :
    at_trainee_guang_talk() : AreaTriggerScript("at_trainee_guang_talk") {}

    bool OnTrigger(Player *player, const AreaTriggerEntry *at)
    {
        std::map<uint64, uint32>::iterator iter = forbiddenPlayers.find(player->GetGUID());
        if(iter != forbiddenPlayers.end())
            return false;
        if(player->GetQuestStatus(QUEST_AYSA_OF_THE_TUSHUI) == QUEST_STATUS_COMPLETE)
        {
            Creature* guang = player->FindNearestCreature(NPC_TRAINEE_GUANG, 500.0f);
            if(guang)
            {
                forbiddenPlayers.insert(std::pair<uint64, uint32>(player->GetGUID(), 120000));
                guang->AI()->Talk(SAY_GUANG, player->GetGUID());
                return true;
            }
        }
        return false;
    }

    void Update(const uint32 uiDiff)
    {
        for (std::map<uint64, uint32>::iterator iter = forbiddenPlayers.begin() ; iter != forbiddenPlayers.end() ; ++iter)
        {
            if(iter->second <= uiDiff)
                forbiddenPlayers.erase(iter);
            else
                iter->second -= uiDiff ;
        }
    }

private :
    std::map<uint64, uint32> forbiddenPlayers ;
};


enum AreaLorvo
{
    NPC_LORVO   = 54943,
    SAY_LORVO   = 0
};

enum AreaAysa
{
    NPC_AYSA                        = 54567,
    QUEST_THE_MISSING_DRIVER_AREA   = 29419,
    SAY_AYSA                        = 0
};

class at_area_7748_talk : public AreaTriggerScript
{
public :
    at_area_7748_talk() : AreaTriggerScript("at_area_7748_talk") {}

    bool OnTrigger(Player *player, const AreaTriggerEntry *at)
    {
        std::map<uint64, uint32>::iterator iter = forbiddenPlayers.find(player->GetGUID());
        if(iter != forbiddenPlayers.end())
            return false;
        if(player->GetQuestStatus(QUEST_AYSA_OF_THE_TUSHUI) == QUEST_STATUS_COMPLETE)
        {
            Creature* lorvo = player->FindNearestCreature(NPC_LORVO, 500.0f);
            if(lorvo)
            {
                forbiddenPlayers.insert(std::pair<uint64, uint32>(player->GetGUID(), 120000));
                lorvo->AI()->Talk(SAY_LORVO, player->GetGUID());
                return true;
            }
        }
        if(player->GetQuestStatus(QUEST_THE_MISSING_DRIVER_AREA) == QUEST_STATUS_COMPLETE)
        {
            Creature* aysa = player->FindNearestCreature(NPC_AYSA, 500.0f);
            if(aysa)
            {
                forbiddenPlayers.insert(std::pair<uint64, uint32>(player->GetGUID(), 120000));
                aysa->AI()->Talk(SAY_AYSA, player->GetGUID());
                return true;
            }
        }
        return false;
    }

    void Update(const uint32 uiDiff)
    {
        for (std::map<uint64, uint32>::iterator iter = forbiddenPlayers.begin() ; iter != forbiddenPlayers.end() ; ++iter)
        {
            if(iter->second <= uiDiff)
                forbiddenPlayers.erase(iter);
            else
                iter->second -= uiDiff ;
        }
    }

private :
    std::map<uint64, uint32> forbiddenPlayers ;
};


enum Area7749
{
    NPC_JI_FIREPAW          = 54568,
    SAY_JI                  = 0,
    QUEST_JI_OF_THE_HUOJIN  = 29522
};

class at_ji_firepaw_talk : public AreaTriggerScript
{
public :
    at_ji_firepaw_talk() : AreaTriggerScript("at_ji_firepaw_talk") {}

    bool OnTrigger(Player *player, const AreaTriggerEntry *at)
    {
        std::map<uint64, uint32>::iterator iter = forbiddenPlayers.find(player->GetGUID());
        if(iter != forbiddenPlayers.end())
            return false;
        if(player->GetQuestStatus(QUEST_JI_OF_THE_HUOJIN) == QUEST_STATUS_COMPLETE)
        {
            Creature* ji = player->FindNearestCreature(NPC_JI_FIREPAW, 500.0f);
            if(ji)
            {
                forbiddenPlayers.insert(std::pair<uint64, uint32>(player->GetGUID(), 120000));
                ji->AI()->Talk(SAY_JI, player->GetGUID());
                return true;
            }
        }
        return false;
    }

    void Update(const uint32 uiDiff)
    {
        for (std::map<uint64, uint32>::iterator iter = forbiddenPlayers.begin() ; iter != forbiddenPlayers.end() ; ++iter)
        {
            if(iter->second <= uiDiff)
                forbiddenPlayers.erase(iter);
            else
                iter->second -= uiDiff ;
        }
    }

private :
    std::map<uint64, uint32> forbiddenPlayers ;
};


enum AreaChia
{
    NPC_CHIA    = 60248,
    SAY_CHIA    = 0
};

enum AreaBrewerLin
{
    NPC_BREWER_LIN          = 60253,
    QUEST_THE_PASSION_AREA  = 29423,
    SAY_LIN                 = 0
};

class at_area_7750_talk : public AreaTriggerScript
{
public :
    at_area_7750_talk() : AreaTriggerScript("at_area_7750_talk") {}

    bool OnTrigger(Player *player, const AreaTriggerEntry *at)
    {
        std::map<uint64, uint32>::iterator iter = forbiddenPlayers.find(player->GetGUID());
        if(iter != forbiddenPlayers.end())
            return false;
        if(player->GetQuestStatus(QUEST_THE_PASSION_AREA) == QUEST_STATUS_INCOMPLETE)
        {
            Creature* lin = player->FindNearestCreature(NPC_BREWER_LIN, 500.0f);
            Creature* chia = player->FindNearestCreature(NPC_CHIA, 500.0f);
            if(lin)
                lin->AI()->Talk(SAY_LIN, player->GetGUID());
            if(chia)
                chia->AI()->Talk(SAY_CHIA, player->GetGUID());

            forbiddenPlayers.insert(std::pair<uint64, uint32>(player->GetGUID(), 120000));
            return true;
        }
        return false;
    }

    void Update(const uint32 uiDiff)
    {
        for (std::map<uint64, uint32>::iterator iter = forbiddenPlayers.begin() ; iter != forbiddenPlayers.end() ; ++iter)
        {
            if(iter->second <= uiDiff)
                forbiddenPlayers.erase(iter);
            else
                iter->second -= uiDiff ;
        }
    }

private :
    std::map<uint64, uint32> forbiddenPlayers ;
};


enum Area7784
{
    QUEST_SINGING_POOLS     = 29521,
    NPC_DENG                = 60249,
    NPC_CAI                 = 60250
};

class at_pop_child_panda : public AreaTriggerScript
{
public :
    at_pop_child_panda() : AreaTriggerScript("at_pop_child_panda") {}

    bool OnTrigger(Player *player, const AreaTriggerEntry *at)
    {
        std::map<uint64, uint32>::iterator iter = forbiddenPlayers.find(player->GetGUID());
        if(iter != forbiddenPlayers.end())
            return false;
        if(player->GetQuestStatus(QUEST_SINGING_POOLS) == QUEST_STATUS_COMPLETE)
        {
            forbiddenPlayers.insert(std::pair<uint64, uint32>(player->GetGUID(), 120000));
            player->SummonCreature(NPC_DENG, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 60000);
            player->SummonCreature(NPC_CAI, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 60000);
            return true;
        }
        return false;
    }

    void Update(const uint32 uiDiff)
    {
        for (std::map<uint64, uint32>::iterator iter = forbiddenPlayers.begin() ; iter != forbiddenPlayers.end() ; ++iter)
        {
            if(iter->second <= uiDiff)
                forbiddenPlayers.erase(iter);
            else
                iter->second -= uiDiff ;
        }
    }

private :
    std::map<uint64, uint32> forbiddenPlayers ;
};



enum Area7258
{
    NPC_DELIVERY_CART_TENDER    = 57712,
    SAY_DELIVERY_CART_TENDER    = 0
};

class at_delivery_cart_talk : public AreaTriggerScript
{
public :
    at_delivery_cart_talk() : AreaTriggerScript("at_delivery_cart_talk") {}

    bool OnTrigger(Player *player, const AreaTriggerEntry *at)
    {
        std::map<uint64, uint32>::iterator iter = forbiddenPlayers.find(player->GetGUID());
        if(iter != forbiddenPlayers.end())
            return false;

        Creature* delivery = player->FindNearestCreature(NPC_DELIVERY_CART_TENDER, 500.0f);
        if(delivery)
        {
            forbiddenPlayers.insert(std::pair<uint64, uint32>(player->GetGUID(), 120000));
            delivery->AI()->Talk(SAY_DELIVERY_CART_TENDER, player->GetGUID());
            return true;
        }
        return false;
    }

    void Update(const uint32 uiDiff)
    {
        for (std::map<uint64, uint32>::iterator iter = forbiddenPlayers.begin() ; iter != forbiddenPlayers.end() ; ++iter)
        {
            if(iter->second <= uiDiff)
                forbiddenPlayers.erase(iter);
            else
                iter->second -= uiDiff ;
        }
    }

private :
    std::map<uint64, uint32> forbiddenPlayers ;
};

enum Area7822
{
    SAY_DELIVERY_CART_TENDER_1    = 1
};

class at_delivery_cart_talk_2 : public AreaTriggerScript
{
public :
    at_delivery_cart_talk_2() : AreaTriggerScript("at_delivery_cart_talk_2") {}

    bool OnTrigger(Player *player, const AreaTriggerEntry *at)
    {
        std::map<uint64, uint32>::iterator iter = forbiddenPlayers.find(player->GetGUID());
        if(iter != forbiddenPlayers.end())
            return false;

        Creature* delivery = player->FindNearestCreature(NPC_DELIVERY_CART_TENDER, 500.0f);
        if(delivery)
        {
            forbiddenPlayers.insert(std::pair<uint64, uint32>(player->GetGUID(), 120000));
            delivery->AI()->Talk(SAY_DELIVERY_CART_TENDER_1, player->GetGUID());
            return true;
        }
        return false;
    }

    void Update(const uint32 uiDiff)
    {
        for (std::map<uint64, uint32>::iterator iter = forbiddenPlayers.begin() ; iter != forbiddenPlayers.end() ; ++iter)
        {
            if(iter->second <= uiDiff)
                forbiddenPlayers.erase(iter);
            else
                iter->second -= uiDiff ;
        }
    }

private :
    std::map<uint64, uint32> forbiddenPlayers ;
};


enum Area7106
{
    QUEST_PASSING_WISDOM    = 29790,
    NPC_AYSA_7106           = 56662,
    NPC_JI_7106             = 56663
};

class at_7106_mongolfiere : public AreaTriggerScript
{
public :
    at_7106_mongolfiere() : AreaTriggerScript("at_7106_mongolfiere") {}

    bool OnTrigger(Player *player, const AreaTriggerEntry *at)
    {
        std::map<uint64, uint32>::iterator iter = forbiddenPlayers.find(player->GetGUID());
        if(iter != forbiddenPlayers.end())
            return false;

        if(player->GetQuestStatus(QUEST_PASSING_WISDOM) == QUEST_STATUS_COMPLETE)
        {
            Creature* aysa = player->FindNearestCreature(NPC_AYSA_7106, 500.0f);
            Creature* ji = player->FindNearestCreature(NPC_JI_7106, 500.0f);
            if(aysa && ji)
            {
                forbiddenPlayers.insert(std::pair<uint64, uint32>(player->GetGUID(), 120000));
                aysa->AI()->DoAction(0);
                ji->AI()->DoAction(0);
                return true;
            }
        }
        return false;
    }

    void Update(const uint32 uiDiff)
    {
        for (std::map<uint64, uint32>::iterator iter = forbiddenPlayers.begin() ; iter != forbiddenPlayers.end() ; ++iter)
        {
            if(iter->second <= uiDiff)
                forbiddenPlayers.erase(iter);
            else
                iter->second -= uiDiff ;
        }
    }

private :
    std::map<uint64, uint32> forbiddenPlayers ;
};


/************************************/
/******* FIN AREATRIGGER ************/
/************************************/



// npc_first_quest_pandaren
class npc_first_quest_pandaren : public CreatureScript
{
public:
    npc_first_quest_pandaren() : CreatureScript("npc_first_quest_pandaren") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_first_quest_pandarenAI (pCreature);
    }

    struct npc_first_quest_pandarenAI : public ScriptedAI
    {
        npc_first_quest_pandarenAI(Creature *c) : ScriptedAI(c)
        {
        }

        uint32 timer;

        void Reset()
        {
            timer = 0;
        }


        void UpdateAI(const uint32 diff)
        {
            if (timer < diff)
            {
                Map::PlayerList const &PlayerList = me->GetMap()->GetPlayers();
                if (!PlayerList.isEmpty())
                {
                    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                    {
                        Player *p = i->getSource();
                        if (p)
                        {
                            if (me->GetExactDist(p) < 40)
								if(p->getRace() == RACE_PANDAREN)
								{
									uint32 questId = 0;

									switch(p->getClass())
									{
									case CLASS_MONK:
										questId = 30039;
										break;
									case CLASS_MAGE:
										questId = 30040;
										break;
									case CLASS_HUNTER:
										questId = 30041;
										break;
									case CLASS_PRIEST:
										questId = 30042;
										break;
									case CLASS_ROGUE:
										questId = 30043;
										break;
									case CLASS_SHAMAN:
										questId = 30044;
										break;
									case CLASS_WARRIOR:
										questId = 30045;
										break;
									default: // Not supposed to happen but in case of
										questId = 30044;
										break;
									}

									Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
									if (quest && !p->hasQuest(questId) && p->CanAddQuest(quest, true) && (p->GetQuestStatus(questId) != QUEST_STATUS_COMPLETE) && (p->GetQuestStatus(questId) != QUEST_STATUS_REWARDED))
									{
										p->AddQuest(quest, NULL);
										p->CompleteQuest(questId);
									}
								}
                        }
                    }
                }

                timer = 2000;
            }
            else
                timer -=diff;
        }
    };
};

/*******************************/
/**The Lesson of Stifled Pride**/
/*******************************/

enum Spells
{
    SPELL_JAB               = 109079,
    SPELL_BLACKOUT_KICK     = 109080
};

enum Texts
{
    SAY_LOOSE   = 0
};

enum Creatures
{
    NPC_ZHI    = 61411
};

class npc_trainee : public CreatureScript
{
public:
    npc_trainee() : CreatureScript("npc_trainee") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_traineeAI(creature);
    }

    struct npc_traineeAI : public ScriptedAI
    {
        npc_traineeAI(Creature* creature) : ScriptedAI(creature)
        {
            creature->SetReactState(REACT_PASSIVE);
        }

        uint32 AttackTimer;
        uint32 DespawnTimer;
        uint32 EmoteTimer;
        bool VerifPV;
        bool Despawn;
        bool Health;
        bool EmoteSpeak;

        void Reset()
        {
            me->SetReactState(REACT_PASSIVE);
            me->HandleEmoteCommand(27);
            AttackTimer = 5000;
            VerifPV = true;
            Despawn = false;
            EmoteSpeak = false;
            Health = true;
            me->setFaction(7);
        }

        void DamageTaken(Unit* caster, uint32 &damage)
        {
            if(damage >= 1)
            {
                if(caster->GetTypeId() == TYPEID_PLAYER && !me->HasReactState(REACT_AGGRESSIVE))
                {
                    me->SetReactState(REACT_AGGRESSIVE);
                    ScriptedAI::AttackStart(caster);
                }
            }

            if (damage >= me->GetHealth())
            {
                damage = 0;
                me->SetHealth(0);
            }

        }

        void JustDied(Unit* /*killer*/)
        {
            me->SetReactState(REACT_PASSIVE);
        }

        void UpdateAI(const uint32 diff)
        {
            if(Despawn)
            {
                if(DespawnTimer <= diff)
                {
                    me->DisappearAndDie();
                    Despawn = false;
                }
                else DespawnTimer -= diff;
            }

            if(EmoteSpeak)
            {
                if(EmoteTimer <= diff)
                {
                    me->HandleEmoteCommand(EMOTE_ONESHOT_BOW);
                    Talk(SAY_LOOSE);
                    EmoteSpeak = false;
                }
                else EmoteTimer -= diff;
            }

            if(!UpdateVictim())
                return;

            if(VerifPV)
            {
                if(AttackTimer <= diff)
                {
                    if(me->GetEntry() == 54586 || me->GetEntry() == 65470)
                        me->CastSpell(me->getVictim(), SPELL_JAB);

                    if(me->GetEntry() == 54587 || me->GetEntry() == 65471)
                        me->CastSpell(me->getVictim(), SPELL_BLACKOUT_KICK);

                    AttackTimer = 5000;
                }
                else AttackTimer -= diff;
            }

            if (me->GetHealthPct() <= 20 && Health)
            {
                VerifPV = false;

                DespawnTimer = 5000;
                Despawn = true;

                EmoteTimer = 2000;
                EmoteSpeak = true;

                if(Unit* player = me->getVictim())
                    if(player->GetTypeId() == TYPEID_PLAYER)
                        player->ToPlayer()->KilledMonsterCredit(54586, 0);

                me->setFaction(35);
                me->StopMoving();
                me->RemoveAllAuras();
                me->GetMotionMaster()->Clear();
                me->CombatStop(true);
                me->DeleteThreatList();

                Health = false;
            }

            DoMeleeAttackIfReady();
        }
    };
};

class npc_zhi : public CreatureScript
{
public:
    npc_zhi() : CreatureScript("npc_zhi") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_zhiAI(creature);
    }

    struct npc_zhiAI : public ScriptedAI
    {
        npc_zhiAI(Creature* creature) : ScriptedAI(creature){}

        uint32 EmoteTimer1;
        uint32 EmoteTimer2;
        uint32 EmoteTimer3;
        uint32 EmoteTimer4;

        uint32 EmoteRepeatTimer1;
        uint32 EmoteRepeatTimer2;
        uint32 EmoteRepeatTimer3;
        uint32 EmoteRepeatTimer4;

        void Reset()
        {
            me->HandleEmoteCommand(27);

            EmoteTimer1 = 5000;
            EmoteTimer2 = 10000;
            EmoteTimer3 = 15000;
            EmoteTimer4 = 20000;

            EmoteRepeatTimer1 = 7000;
            EmoteRepeatTimer2 = 12000;
            EmoteRepeatTimer3 = 17000;
            EmoteRepeatTimer4 = 22000;
        }


        void UpdateAI(const uint32 diff)
        {
            std::list<Creature*> creatures1;
            GetCreatureListWithEntryInGrid(creatures1, me, 65471, 50.0f);

            std::list<Creature*> creatures2;
            GetCreatureListWithEntryInGrid(creatures2, me, 54587, 50.0f);

            if(EmoteTimer1 <= diff)
            {
                me->HandleEmoteCommand(480);
                EmoteTimer1 = 20000;
            }
            else EmoteTimer1 -= diff;

            if(EmoteTimer2 <= diff)
            {
                me->HandleEmoteCommand(508);
                EmoteTimer2 = 20000;
            }
            else EmoteTimer2 -= diff;

            if(EmoteTimer3 <= diff)
            {
                me->HandleEmoteCommand(439);
                EmoteTimer3 = 20000;
            }
            else EmoteTimer3 -= diff;

            if(EmoteTimer4 <= diff)
            {
                me->HandleEmoteCommand(60);
                EmoteTimer4 = 20000;
            }
            else EmoteTimer4 -= diff;

            if(EmoteRepeatTimer1 <= diff)
            {
                for(std::list<Creature*>::const_iterator iter = creatures1.begin() ; iter != creatures1.end() ; ++iter)
                    if (Creature* tushui = *iter)
                        if(!tushui->isInCombat())
                            tushui->HandleEmoteCommand(480);

                for(std::list<Creature*>::const_iterator iter = creatures2.begin() ; iter != creatures2.end() ; ++iter)
                    if (Creature* tushui = *iter)
                        if(!tushui->isInCombat())
                            tushui->HandleEmoteCommand(480);

                EmoteRepeatTimer1 = 20000;
            }
            else EmoteRepeatTimer1 -= diff;

            if(EmoteRepeatTimer2 <= diff)
            {
                for(std::list<Creature*>::const_iterator iter = creatures1.begin() ; iter != creatures1.end() ; ++iter)
                    if (Creature* tushui = *iter)
                        if(!tushui->isInCombat())
                            tushui->HandleEmoteCommand(508);

                for(std::list<Creature*>::const_iterator iter = creatures2.begin() ; iter != creatures2.end() ; ++iter)
                    if (Creature* tushui = *iter)
                        if(!tushui->isInCombat())
                            tushui->HandleEmoteCommand(508);

                EmoteRepeatTimer2 = 20000;
            }
            else EmoteRepeatTimer2 -= diff;

            if(EmoteRepeatTimer3 <= diff)
            {
                for(std::list<Creature*>::const_iterator iter = creatures1.begin() ; iter != creatures1.end() ; ++iter)
                    if (Creature* tushui = *iter)
                        if(!tushui->isInCombat())
                            tushui->HandleEmoteCommand(439);

                for(std::list<Creature*>::const_iterator iter = creatures2.begin() ; iter != creatures2.end() ; ++iter)
                    if (Creature* tushui = *iter)
                        if(!tushui->isInCombat())
                            tushui->HandleEmoteCommand(439);

                EmoteRepeatTimer3 = 20000;
            }
            else EmoteRepeatTimer3 -= diff;

            if(EmoteRepeatTimer4 <= diff)
            {
                for(std::list<Creature*>::const_iterator iter = creatures1.begin() ; iter != creatures1.end() ; ++iter)
                    if (Creature* tushui = *iter)
                        if(!tushui->isInCombat())
                            tushui->HandleEmoteCommand(60);

                for(std::list<Creature*>::const_iterator iter = creatures2.begin() ; iter != creatures2.end() ; ++iter)
                    if (Creature* tushui = *iter)
                        if(!tushui->isInCombat())
                            tushui->HandleEmoteCommand(60);

                EmoteRepeatTimer4 = 20000;
            }
            else EmoteRepeatTimer4 -= diff;
        }
    };
};

/*#####
## at_The Missing Driver
#####*/

enum eTheMissingDriver
{
    QUEST_THE_MISSING_DRIVER    = 29419,
    NPC_AMBERLEAF_SCAMP         = 54130,
    NPC_MIN_DIMWIND             = 54855,
    NPC_MIN_DIMWIND_POP         = 56503,
    SAY_1                       = 0

};

class areatrigger_at_the_missing_driver : public AreaTriggerScript
{
    public:

        areatrigger_at_the_missing_driver(): AreaTriggerScript("areatrigger_at_the_missing_driver")
        {
        }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/)
        {
            if (player->isAlive() && !player->isInCombat())
            {
                if (player->GetQuestStatus(QUEST_THE_MISSING_DRIVER) == QUEST_STATUS_INCOMPLETE)
                {
                    if(Creature* min = player->FindNearestCreature(NPC_MIN_DIMWIND, 20.0f))
                    {
                        std::list<Creature*> creatures;
                        GetCreatureListWithEntryInGrid(creatures, player, NPC_AMBERLEAF_SCAMP, 20.0f);

                        for(std::list<Creature*>::const_iterator iter = creatures.begin() ; iter != creatures.end() ; ++iter)
                        {
                            (*iter)->AI()->Talk(SAY_1);
                            (*iter)->DespawnOrUnsummon(3000);
                        }

                        min->DisappearAndDie();
                        player->SummonCreature(NPC_MIN_DIMWIND_POP, min->GetPositionX(), min->GetPositionY(), min->GetPositionZ(), 2.08f, TEMPSUMMON_TIMED_DESPAWN, 120000);
                        player->KilledMonsterCredit(54855);

                        return true;
                    }
                }

                return false;
            }
            return false;
        }
};

/*######
## npc_min_dimwind_pop
######*/

enum eMinDimwind
{
    SAY_DIMWIND_1    = 0,
    SAY_DIMWIND_2    = 1,
    SAY_DIMWIND_3    = 2
};

class npc_min_dimwind_pop : public CreatureScript
{
public:
    npc_min_dimwind_pop(): CreatureScript("npc_min_dimwind_pop") { }

    struct npc_min_dimwind_popAI : public npc_escortAI
    {
        npc_min_dimwind_popAI(Creature* creature) : npc_escortAI(creature) {}

        void Reset()
        {
        }

        void WaypointReached(uint32 waypointId)
        {
            Player* player = GetPlayerForEscort();

            switch (waypointId)
            {
                case 1:
                    Talk(SAY_DIMWIND_1);
                    break;
                case 2:
                    Talk(SAY_DIMWIND_2);
                    break;
                case 3:
                    Talk(SAY_DIMWIND_3);
                    break;
                case 10:
                    me->DespawnOrUnsummon();
                    break;

            }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            npc_escortAI::UpdateAI(uiDiff);

            if (UpdateVictim())
                return;

            Start(false, true);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_min_dimwind_popAI(creature);
    }
};

/*######
## npc_aysa_cloudsinger_pop
######*/

enum eAysaCloudsinger
{
    SAY_AYSA_1    = 0
};

class npc_aysa_cloudsinger_pop : public CreatureScript
{
public:
    npc_aysa_cloudsinger_pop(): CreatureScript("npc_aysa_cloudsinger_pop") { }

    struct npc_aysa_cloudsinger_popAI : public npc_escortAI
    {
        npc_aysa_cloudsinger_popAI(Creature* creature) : npc_escortAI(creature) {}

        void Reset()
        {
        }

        void WaypointReached(uint32 waypointId)
        {
            Player* player = GetPlayerForEscort();

            switch (waypointId)
            {
                case 1:
                    Talk(SAY_AYSA_1);
                    break;
                case 2:
                    me->GetMotionMaster()->MoveJump(1197.14f, 3492.05f, 91.33f, 20, 20);
                    break;
                case 4:
                    me->GetMotionMaster()->MoveJump(1193.65f, 3478.94f, 108.50f, 20, 20);
                    break;
                case 6:
                    me->GetMotionMaster()->MoveJump(1188.2f, 3456.19f, 102.90f, 20, 20);
                    break;
                case 10:
                    me->DespawnOrUnsummon();
                    break;

            }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            npc_escortAI::UpdateAI(uiDiff);

            if (UpdateVictim())
                return;

            Start(false, true);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_aysa_cloudsinger_popAI(creature);
    }
};



/*************************************/
/********The Way of the Tushui********/
/*************************************/

const Position SummonPositions[] =
{
    {1180.69f, 3450.00f, 103.00f, 3.50f},
    {1181.10f, 3447.40f, 102.70f, 3.50f},
    {1182.05f, 3444.38f, 102.70f, 3.50f},
    {1183.14f, 3440.85f, 102.70f, 3.50f}
};

enum Quests
{
    QUEST_THE_WAY_OF_THE_TUSHUI     = 29414
};

enum Events
{
    EVENT_ADD_POWER     = 1,
    EVENT_SUMMON_NPCS   = 2,
    EVENT_INTRO         = 3,
    EVENT_LOOK_PLAYERS  = 4
};

enum SpellsAysa
{
    SPELL_MEDITATION_BAR = 116421
};

enum Npcs
{
    NPC_MASTER_LI_FEI   = 54856,
    MOB_SCAMP           = 59637
};

enum SaysLiFei
{
    LI_FEI_SPEECH_1     = 0,
    LI_FEI_SPEECH_2     = 1,
    LI_FEI_SPEECH_3     = 2,
    LI_FEI_SPEECH_4     = 3,
    LI_FEI_SPEECH_5     = 4,
    LI_FEI_SPEECH_6     = 5,
    LI_FEI_SPEECH_7     = 6
};

enum SaysAysa
{
    AYSA_SAY_INTRO  = 0,
    AYSA_SAY_OUTRO  = 1
};

enum Area
{
    AREA_MEDITATION = 5848
};

class npc_aysa_cloudsinger_meditation : public CreatureScript
{
public :
    npc_aysa_cloudsinger_meditation() : CreatureScript("npc_aysa_cloudsinger_meditation") {    }

    struct npc_aysa_cloudsinger_meditation_AI : public ScriptedAI
    {
    public :
        npc_aysa_cloudsinger_meditation_AI(Creature* c) : ScriptedAI(c), Summons(me)
        {
        }

        bool isStarted ;
        uint32 StartTimer;
        EventMap events ;
        Creature* LiFei;
        SummonList Summons;

        void Reset()
        {
            isStarted = false ;
            Summons.DespawnAll();
            StartTimer = 1000;
        }

        void JustSummoned(Creature* Summoned)
        {
            Summons.Summon(Summoned);
        }

        void UpdateAI(const uint32 diff)
        {
            if(StartTimer <= diff)
            {
                if(!isStarted)
                {
                    if(isAPlayerWithQuestInDist())
                    {
                        StartEvent();
                        isStarted = true ;
                    }
                    else
                        return ;
                }

                StartTimer = 1000;
            }
            else StartTimer -= diff;

            events.Update(diff);

            while(uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
                case EVENT_INTRO :
                    DoIntro();
                    break ;

                case EVENT_ADD_POWER :
                    AddPowerToPlayersOnMap();
                    events.ScheduleEvent(EVENT_ADD_POWER, 2000);
                    break ;

                case EVENT_SUMMON_NPCS :
                    SummonNpcs();
                    events.ScheduleEvent(EVENT_SUMMON_NPCS, 10000);
                    break ;

                case EVENT_LOOK_PLAYERS :
                    if(isAPlayerWithQuestInDist())
                        events.ScheduleEvent(EVENT_LOOK_PLAYERS, 1000);
                    else
                        EndEvent();
                    break ;

                default :
                    break ;
                }
            }
        }

        void StartEvent()
        {
            events.ScheduleEvent(EVENT_INTRO, 3000);
            events.ScheduleEvent(EVENT_ADD_POWER, 4000);
            events.ScheduleEvent(EVENT_SUMMON_NPCS, 2000);
            events.ScheduleEvent(EVENT_LOOK_PLAYERS, 3000);
        }

        void EndEvent()
        {
            events.CancelEvent(EVENT_ADD_POWER);
            events.CancelEvent(EVENT_INTRO);
            events.CancelEvent(EVENT_SUMMON_NPCS);
            events.CancelEvent(EVENT_LOOK_PLAYERS);
            isStarted = false;
            if(LiFei)
                LiFei->DespawnOrUnsummon();
            Reset();
        }

        bool isAPlayerWithQuestInDist()
        {
            Map* map = me->GetMap();
            if(map)
            {
                Map::PlayerList const& players = map->GetPlayers();

                if(players.isEmpty())
                    return false ;

                for(Map::PlayerList::const_iterator iter = players.begin() ; iter != players.end() ; ++iter)
                {
                    Player *player = iter->getSource();
                    if(player)
                    {
                        if (player->isAlive() && player->GetQuestStatus(QUEST_THE_WAY_OF_THE_TUSHUI) == QUEST_STATUS_INCOMPLETE)
                        {
                            if(player->GetAreaId() == AREA_MEDITATION && player->IsInDist2d(me, 35))
                            {
                                if(!player->HasAura(SPELL_MEDITATION_BAR))
                                {
                                    player->CastSpell(player, SPELL_MEDITATION_BAR, true);
                                    player->SetMaxPower(POWER_ALTERNATE_POWER, 90);
                                    player->SetPower(POWER_ALTERNATE_POWER, 0);
                                }
                                return true;
                            }
                        }
                        if (!player->IsInDist2d(me, 35) && player->HasAura(SPELL_MEDITATION_BAR))
                        {
                            player->RemoveAura(SPELL_MEDITATION_BAR);
                        }
                    }
                }
            }
            return false ;
        }

        void AddPowerToPlayersOnMap()
        {
            if(!isStarted)
                return ;

            Map* map = me->GetMap();
            if(map)
            {
                Map::PlayerList const& players = map->GetPlayers();

                if(players.isEmpty())
                    return ;

                for(Map::PlayerList::const_iterator iter = players.begin() ; iter != players.end() ; ++iter)
                {
                    Player* player = iter->getSource();
                    if(player)
                    {
                        if (player->isAlive() && player->GetQuestStatus(QUEST_THE_WAY_OF_THE_TUSHUI) == QUEST_STATUS_INCOMPLETE)
                        {
                            if(player->GetAreaId() == AREA_MEDITATION && player->IsInDist2d(me, 35))
                            {
                                if(player->HasAura(SPELL_MEDITATION_BAR))
                                {
                                    AddPower(player);
                                }
                            }
                        }
                    }
                }
            }
        }

        void AddPower(Player* player)
        {
            if(!isStarted)
                return ;

            if(player)
            {
                player->ModifyPower(POWER_ALTERNATE_POWER, +2);

                switch(player->GetPower(POWER_ALTERNATE_POWER))
                {
                    case 20 :
                        LiFei->AI()->Talk(LI_FEI_SPEECH_1, player->GetGUID());
                        break;
                    case 40 :
                        LiFei->AI()->Talk(LI_FEI_SPEECH_2, player->GetGUID());
                        break ;
                    case 50 :
                        LiFei->AI()->Talk(LI_FEI_SPEECH_3, player->GetGUID());
                        break ;
                    case 60 :
                        LiFei->AI()->Talk(LI_FEI_SPEECH_4, player->GetGUID());
                        break;
                    case 70 :
                        LiFei->AI()->Talk(LI_FEI_SPEECH_5, player->GetGUID());
                        break ;
                    case 80 :
                        LiFei->AI()->Talk(LI_FEI_SPEECH_6, player->GetGUID());
                        break;
                    case 90 :
                        LiFei->AI()->Talk(LI_FEI_SPEECH_7, player->GetGUID());
                        player->KilledMonsterCredit(NPC_MASTER_LI_FEI);
                        player->RemoveAura(SPELL_MEDITATION_BAR);
                        Talk(AYSA_SAY_OUTRO, player->GetGUID());
                        break ;

                default :
                    break ;
                }
            }
        }

        void SummonNpcs()
        {
            if(!isStarted)
                return ;

            uint8 number = (1 + (rand() % 3));

            for(uint8 i = 0 ; i < number ; ++i)
            {
                Creature* scamp = me->SummonCreature(MOB_SCAMP, SummonPositions[i], TEMPSUMMON_TIMED_DESPAWN, 180000);
                    scamp->GetMotionMaster()->MovePoint(1, 1144.12f, 3435.47f, 105.3f);
            }
        }

        void DoIntro()
        {
            if(!isStarted)
                return ;

            LiFei = me->SummonCreature(NPC_MASTER_LI_FEI, 1130.19f, 3435.37f, 106.00f, 0.19f);
            Talk(AYSA_SAY_INTRO);
        }
    };

    CreatureAI* GetAI(Creature *c) const
    {
        return new npc_aysa_cloudsinger_meditation_AI(c);
    }
};


/*************************************/
/********Fanning the Flames********/
/*************************************/

enum SpellsAir
{
    SPELL_LIGHTNING_BOLT    = 73212,
    SPELL_LIGHTNING_CLOUD   = 108693
};

class npc_living_air: public CreatureScript
{
public:
    npc_living_air() : CreatureScript("npc_living_air") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_living_airAI(creature);
    }

    struct npc_living_airAI : public ScriptedAI
    {
            npc_living_airAI(Creature* creature) : ScriptedAI(creature) {}

            uint32 Bolt1_timer;
            uint32 Bolt2_timer;
            uint32 Bolt3_timer;
            uint32 Cloud_timer;

            void Reset()
            {
                Bolt1_timer = 1000;
                Bolt2_timer = 4200;
                Bolt3_timer = 7400;
                Cloud_timer = 10600;
            }

            void UpdateAI(const uint32 diff)
            {
                if(!UpdateVictim())
                    return;

                if (Bolt1_timer <= diff)
                {
                    me->CastSpell(me->getVictim(), SPELL_LIGHTNING_BOLT, false);
                    Bolt1_timer = 14000;
                }
                else Bolt1_timer -= diff;

                if (Bolt2_timer <= diff)
                {
                    me->CastSpell(me->getVictim(), SPELL_LIGHTNING_BOLT, false);
                    Bolt2_timer = 14000;
                }
                else Bolt2_timer -= diff;

                if (Bolt3_timer <= diff)
                {
                    me->CastSpell(me->getVictim(), SPELL_LIGHTNING_BOLT, false);
                    Bolt3_timer = 14000;
                }
                else Bolt3_timer -= diff;

                if (Cloud_timer <= diff)
                {
                    me->CastSpell(me->getVictim(), SPELL_LIGHTNING_CLOUD, false);
                    Cloud_timer = 14000;
                }
                else Cloud_timer -= diff;

                DoMeleeAttackIfReady();
            }
    };
};


/*************************************/
/****Only the Worthy Shall Pass****/
/*************************************/

class spell_blessing_flamme_panda: public SpellScriptLoader
{
    public:
        spell_blessing_flamme_panda() : SpellScriptLoader("spell_blessing_flamme_panda") { }

        class spell_blessing_flamme_panda_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_blessing_flamme_panda_AuraScript);

            bool DoCheckTarget(Unit* target)
            {
                if(target)
                {
                    if(target->ToCreature() && target->ToCreature()->GetEntry() == 54900)
                        return true;

                    if(target->ToPlayer() && target->ToPlayer()->GetQuestStatus(29421) == QUEST_STATUS_INCOMPLETE)
                        return true;
                }
                return false;
            }

            void Register()
            {
                DoCheckAreaTarget += AuraCheckAreaTargetFn(spell_blessing_flamme_panda_AuraScript::DoCheckTarget);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_blessing_flamme_panda_AuraScript();
        }
};


/*************************************/
/****The Passion of Shen-zin Su****/
/*************************************/

enum HuoEnum
{
    NPC_SHANG_XI        = 54786,
    SAY_SHANG_XI        = 0,
    QUEST_THE_PASSION   = 29423
};

class npc_huo_escort: public CreatureScript
{
public:
    npc_huo_escort() : CreatureScript("npc_huo_escort") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_huo_escortAI(creature);
    }

    struct npc_huo_escortAI : public ScriptedAI
    {
            npc_huo_escortAI(Creature* creature) : ScriptedAI(creature) {}

            uint32 TestTimer;

            void Reset()
            {
                TestTimer = 1000;
                if (Unit* owner = me->GetOwner())
                    me->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
            }

            void MoveInLineOfSight(Unit* who)
            {
                if (who->GetEntry() == NPC_SHANG_XI)
                {
                    if (me->IsWithinDistInMap(who, 10.0f))
                    {
                        if (Creature* talker = who->ToCreature())
                            talker->AI()->Talk(SAY_SHANG_XI);

                        if (Unit* owner = me->GetOwner())
                            if(Player* player = owner->ToPlayer())
                                player->KilledMonsterCredit(61128);

                        me->DespawnOrUnsummon();
                    }
                }
            }

            void UpdateAI(const uint32 diff)
            {
                if (TestTimer <= diff)
                {
                    if (Unit* owner = me->GetOwner())
                        if(owner->ToPlayer() && owner->ToPlayer()->GetQuestStatus(QUEST_THE_PASSION) == QUEST_STATUS_NONE)
                            me->DespawnOrUnsummon();

                    TestTimer = 1000;
                }
                else TestTimer -= diff;
            }
    };
};


/*************************************/
/*********The Singing Pools**********/
/*************************************/

enum ChildDeng
{
    SAY_DENG_0      = 0,
    SAY_DENG_1      = 1,
    SAY_DENG_2      = 2
};

class npc_deng_child: public CreatureScript
{
public:
    npc_deng_child() : CreatureScript("npc_deng_child") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_deng_childAI(creature);
    }

    struct npc_deng_childAI : public ScriptedAI
    {
            npc_deng_childAI(Creature* creature) : ScriptedAI(creature) {}

            uint32 Say0_timer;
            uint32 Say1_timer;
            uint32 Say2_timer;
            uint32 Despawn_timer;

            void Reset()
            {
                Say0_timer = 9000;
                Say1_timer = 25000;
                Say2_timer = 41000;
                Despawn_timer = 45000;

                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                    me->GetMotionMaster()->MoveFollow(summoner, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
            }

            void UpdateAI(const uint32 diff)
            {
                if (Say0_timer <= diff)
                {
                    Talk(SAY_DENG_0);
                    Say0_timer = 60000;
                }
                else Say0_timer -= diff;

                if (Say1_timer <= diff)
                {
                    Talk(SAY_DENG_1);
                    Say1_timer = 60000;
                }
                else Say1_timer -= diff;

                if (Say2_timer <= diff)
                {
                    Talk(SAY_DENG_2);
                    Say2_timer = 60000;
                }
                else Say2_timer -= diff;

                if (Despawn_timer <= diff)
                {
                    me->DespawnOrUnsummon();
                    Despawn_timer = 60000;
                }
                else Despawn_timer -= diff;
            }
    };
};

enum ChildCai
{
    SAY_CAI_0      = 0,
    SAY_CAI_1      = 1,
    SAY_CAI_2      = 2,
    SAY_CAI_3      = 3
};

class npc_cai_child: public CreatureScript
{
public:
    npc_cai_child() : CreatureScript("npc_cai_child") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_cai_childAI(creature);
    }

    struct npc_cai_childAI : public ScriptedAI
    {
            npc_cai_childAI(Creature* creature) : ScriptedAI(creature) {}

            uint32 Say0_timer;
            uint32 Say1_timer;
            uint32 Say2_timer;
            uint32 Say3_timer;
            uint32 Despawn_timer;

            void Reset()
            {
                Say0_timer = 1000;
                Say1_timer = 17000;
                Say2_timer = 33000;
                Say3_timer = 41000;
                Despawn_timer = 45000;

                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                    me->GetMotionMaster()->MoveFollow(summoner, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
            }

            void UpdateAI(const uint32 diff)
            {
                if (Say0_timer <= diff)
                {
                    Talk(SAY_CAI_0);
                    Say0_timer = 60000;
                }
                else Say0_timer -= diff;

                if (Say1_timer <= diff)
                {
                    Talk(SAY_CAI_1);
                    Say1_timer = 60000;
                }
                else Say1_timer -= diff;

                if (Say2_timer <= diff)
                {
                    Talk(SAY_CAI_2);
                    Say2_timer = 60000;
                }
                else Say2_timer -= diff;

                if (Say3_timer <= diff)
                {
                    Talk(SAY_CAI_3);
                    Say3_timer = 60000;
                }
                else Say3_timer -= diff;

                if (Despawn_timer <= diff)
                {
                    me->DespawnOrUnsummon();
                    Despawn_timer = 60000;
                }
                else Despawn_timer -= diff;
            }
    };
};


/*************************************/
/************** Etang ****************/
/*************************************/

enum AreaPole
{
    SPELL_TEST  = 102938
};

class npc_balance_pole: public CreatureScript
{
public:
    npc_balance_pole() : CreatureScript("npc_balance_pole") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_balance_poleAI(creature);
    }

    struct npc_balance_poleAI : public ScriptedAI
    {
            npc_balance_poleAI(Creature* creature) : ScriptedAI(creature) {}

            void Reset()
            {
                me->SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, 15);
                me->SetFloatValue(UNIT_FIELD_COMBATREACH, 15);
            }

            void UpdateAI(const uint32 diff)
            {
                if (me->GetVehicleKit())
                {
                    if (Unit* passenger = me->GetVehicleKit()->GetPassenger(0))
                    {
                        if (passenger->GetTypeId() == TYPEID_PLAYER)
                        {
                            if(passenger->HasAura(SPELL_TEST))
                                passenger->RemoveAurasDueToSpell(SPELL_TEST);
                        }
                    }
                }
            }
    };
};


class npc_balance_pole_finish: public CreatureScript
{
public:
    npc_balance_pole_finish() : CreatureScript("npc_balance_pole_finish") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_balance_pole_finishAI(creature);
    }

    struct npc_balance_pole_finishAI : public ScriptedAI
    {
            npc_balance_pole_finishAI(Creature* creature) : ScriptedAI(creature) {}

            void Reset()
            {
                me->SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, 10);
                me->SetFloatValue(UNIT_FIELD_COMBATREACH, 10);
            }

            void UpdateAI(const uint32 diff)
            {
                if (me->GetVehicleKit())
                {
                    if (Unit* passenger = me->GetVehicleKit()->GetPassenger(0))
                    {
                        if (passenger->GetTypeId() == TYPEID_PLAYER)
                        {
                            passenger->ExitVehicle();
                            passenger->GetMotionMaster()->MoveJump(935.44f, 3341.04f, 124.00f, 10, 10);
                        }
                    }
                }
            }
    };
};


enum SpellMonk
{
    SPELL_THROW_ROCK    = 109308
};

enum TextMonk
{
    SAY_MONK   = 0
};

class npc_tushui_monk : public CreatureScript
{
public:
    npc_tushui_monk() : CreatureScript("npc_tushui_monk") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tushui_monkAI(creature);
    }

    struct npc_tushui_monkAI : public ScriptedAI
    {
        npc_tushui_monkAI(Creature* creature) : ScriptedAI(creature)
        {
            creature->SetReactState(REACT_PASSIVE);
        }

        uint32 AttackTimer;
        uint32 DespawnTimer;
        bool VerifPV;
        bool Despawn;
        bool Health;

        void Reset()
        {
            me->SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, 5);
            me->SetFloatValue(UNIT_FIELD_COMBATREACH, 5);
            me->SetReactState(REACT_PASSIVE);
            AttackTimer = 1000;
            VerifPV = true;
            Despawn = false;
            Health = true;
            me->setFaction(7);
        }

        void DamageTaken(Unit* caster, uint32 &damage)
        {
            if(damage >= 1)
            {
                if(caster->GetTypeId() == TYPEID_PLAYER && !me->HasReactState(REACT_AGGRESSIVE))
                {
                    me->SetReactState(REACT_AGGRESSIVE);
                    ScriptedAI::AttackStart(caster);
                }
            }

            if (damage >= me->GetHealth())
            {
                damage = 0;
                me->SetHealth(0);
            }

        }

        void JustDied(Unit* /*killer*/)
        {
            me->SetReactState(REACT_PASSIVE);
        }

        void UpdateAI(const uint32 diff)
        {
            if(Despawn)
            {
                if(DespawnTimer <= diff)
                {
                    me->DisappearAndDie();
                    Despawn = false;
                }
                else DespawnTimer -= diff;
            }

            if(!UpdateVictim())
                return;

            if(VerifPV)
            {
                if(AttackTimer <= diff)
                {
                    me->CastSpell(me->getVictim(), SPELL_THROW_ROCK);
                    AttackTimer = 3000;
                }
                else AttackTimer -= diff;
            }

            if (me->GetHealthPct() <= 20 && Health)
            {
                VerifPV = false;

                DespawnTimer = 2000;
                Despawn = true;

                if(Unit* player = me->getVictim())
                    if(player->GetTypeId() == TYPEID_PLAYER)
                        player->ToPlayer()->KilledMonsterCredit(55019, 0);

                me->setFaction(35);
                me->StopMoving();
                me->RemoveAllAuras();
                me->GetMotionMaster()->Clear();
                me->CombatStop(true);
                me->DeleteThreatList();

                Health = false;
            }

            DoMeleeAttackIfReady();
        }
    };
};


/*######
## npc_jojo_ironbrow
######*/

enum JojoIronbrow
{
    SPELL_REEDS_CAST    = 129272,
    SAY_JOJO_1          = 0,
    SAY_JOJO_2          = 1
};

class npc_jojo_ironbrow : public CreatureScript
{
public:
    npc_jojo_ironbrow(): CreatureScript("npc_jojo_ironbrow") { }

    struct npc_jojo_ironbrowAI : public npc_escortAI
    {
        npc_jojo_ironbrowAI(Creature* creature) : npc_escortAI(creature) {}

        void Reset()
        {
        }

        void WaypointReached(uint32 waypointId)
        {
            Player* player = GetPlayerForEscort();

            switch (waypointId)
            {
                case 1:
                    Talk(SAY_JOJO_1);
                    break;
                case 2:
                    me->CastSpell(me, SPELL_REEDS_CAST, true);
                    break;
                case 3:
                    Talk(SAY_JOJO_2);
                    break;
                case 10:
                    me->DespawnOrUnsummon();
                    break;

            }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            npc_escortAI::UpdateAI(uiDiff);

            if (UpdateVictim())
                return;

            Start(false, true);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_jojo_ironbrowAI(creature);
    }
};

/*#####
## at_the_spirit_of_water
#####*/

enum eSpiritofwater
{
    QUEST_THE_SPIRIT_OF_WATER   = 29678,
    SPELL_REFLECTION_CREDIT     = 108590
};

class at_the_spirit_of_water : public AreaTriggerScript
{
    public:

        at_the_spirit_of_water(): AreaTriggerScript("at_the_spirit_of_water")
        {
        }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/)
        {
            if (player->isAlive())
            {
                if (player->GetQuestStatus(QUEST_THE_SPIRIT_OF_WATER) == QUEST_STATUS_INCOMPLETE)
                {
                    player->CastSpell(player, SPELL_REFLECTION_CREDIT, true);
                    return true;
                }
                return false;
            }
            return false;
        }
};


/*************************************/
/********* A New Friend ***********/
/*************************************/

enum NewFriend
{
    SPELL_WATER_SPOUT       = 117063,

    SPELL_AURA_GEYSER       = 117057,
    SPELL_WARNING_GEYSER    = 116695,
    SPELL_BURST_GEYSER      = 116696,
    NPC_WATER_BUNNY         = 60488,

    NPC_AYSA_REFLEXION      = 54975,
    SAY_AYSA_REFLEXION_1    = 0,
    SAY_AYSA_REFLEXION_2    = 1,

    QUEST_THE_SOURCE        = 29680
};

class npc_shu_reflexion : public CreatureScript
{
public:
    npc_shu_reflexion() : CreatureScript("npc_shu_reflexion") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shu_reflexionAI(creature);
    }

    struct npc_shu_reflexionAI : public ScriptedAI
    {
        npc_shu_reflexionAI(Creature* creature) : ScriptedAI(creature){}

        uint32 WaterTimer;
        uint32 Jump1Timer;
        uint32 Jump2Timer;
        uint32 Jump3Timer;
        uint32 Jump4Timer;

        void Reset()
        {
            WaterTimer = 3000;
            Jump1Timer = 10000;
            Jump2Timer = 20000;
            Jump3Timer = 30000;
            Jump4Timer = 40000;
        }

        void UpdateAI(const uint32 diff)
        {
            if(WaterTimer <= diff)
            {
                me->CastSpell(me, SPELL_WATER_SPOUT, false);
                WaterTimer = 10000;
            }
            else WaterTimer -= diff;

            if(Jump1Timer <= diff)
            {
                me->GetMotionMaster()->MoveJump(1120.23f, 2883.21f, 96.50f, 10, 10);
                me->SummonCreature(NPC_WATER_BUNNY, 1117.72f, 2875.84f, 92.18f, 0, TEMPSUMMON_TIMED_DESPAWN, 20000);
                Jump1Timer = 40000;
            }
            else Jump1Timer -= diff;

            if(Jump2Timer <= diff)
            {
                me->GetMotionMaster()->MoveJump(1127.46f, 2859.07f, 97.60f, 10, 10);
                me->SummonCreature(NPC_WATER_BUNNY, 1131.24f, 2866.60f, 92.18f, 0, TEMPSUMMON_TIMED_DESPAWN, 20000);
                Jump2Timer = 40000;
            }
            else Jump2Timer -= diff;

            if(Jump3Timer <= diff)
            {
                me->GetMotionMaster()->MoveJump(1111.08f, 2850.20f, 94.70f, 10, 10);
                me->SummonCreature(NPC_WATER_BUNNY, 1110.47f, 2858.06f, 92.18f, 0, TEMPSUMMON_TIMED_DESPAWN, 20000);
                Jump3Timer = 40000;
            }
            else Jump3Timer -= diff;

            if(Jump4Timer <= diff)
            {
                me->GetMotionMaster()->MoveJump(1100.69f, 2881.31f, 94.00f, 10, 10);
                me->SummonCreature(NPC_WATER_BUNNY, 1097.62f, 2875.08f, 92.18f, 0, TEMPSUMMON_TIMED_DESPAWN, 20000);
                Jump4Timer = 40000;
            }
            else Jump4Timer -= diff;
        }
    };
};


class npc_water_spout_bunny : public CreatureScript
{
public:
    npc_water_spout_bunny() : CreatureScript("npc_water_spout_bunny") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_water_spout_bunnyAI(creature);
    }

    struct npc_water_spout_bunnyAI : public ScriptedAI
    {
        npc_water_spout_bunnyAI(Creature* creature) : ScriptedAI(creature){}

        uint32 WarningTimer;
        uint32 AuraTimer;

        void Reset()
        {
            WarningTimer = 3000;
            AuraTimer = 7000;
        }

        void UpdateAI(const uint32 diff)
        {
            if(WarningTimer <= diff)
            {
                me->CastSpell(me, SPELL_WARNING_GEYSER, true);
                WarningTimer = 40000;
            }
            else WarningTimer -= diff;

            if(AuraTimer <= diff)
            {
                me->CastSpell(me, SPELL_AURA_GEYSER, true);
                me->CastSpell(me, SPELL_BURST_GEYSER, true);
                me->RemoveAurasDueToSpell(SPELL_WARNING_GEYSER);
                AuraTimer = 40000;
            }
            else AuraTimer -= diff;
        }
    };
};

class npc_shu_escort: public CreatureScript
{
public:
    npc_shu_escort() : CreatureScript("npc_shu_escort") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shu_escortAI(creature);
    }

    struct npc_shu_escortAI : public ScriptedAI
    {
            npc_shu_escortAI(Creature* creature) : ScriptedAI(creature) {}

            uint32 Talk1Timer;
            uint32 Talk2Timer;
            uint32 DespawnTimer;

            void Reset()
            {
                Talk1Timer = 1000;
                Talk2Timer = 6000;
                DespawnTimer = 1000;
            }

            void UpdateAI(const uint32 diff)
            {
                if (Talk1Timer <= diff)
                {
                    if(me->GetAreaId() == 5862)
                        if(Creature* aysa = me->FindNearestCreature(NPC_AYSA_REFLEXION, 100.00f, true))
                            aysa->AI()->Talk(SAY_AYSA_REFLEXION_1);

                    Talk1Timer = 120000;
                }
                else Talk1Timer -= diff;

                if (Talk2Timer <= diff)
                {
                    if(me->GetAreaId() == 5862)
                        if(Creature* aysa = me->FindNearestCreature(NPC_AYSA_REFLEXION, 100.00f, true))
                            aysa->AI()->Talk(SAY_AYSA_REFLEXION_2);

                    Talk2Timer = 120000;
                }
                else Talk2Timer -= diff;

                if (DespawnTimer <= diff)
                {
                    if(Unit* owner = me->GetOwner())
                        if(owner->ToPlayer() && owner->ToPlayer()->GetQuestStatus(QUEST_THE_SOURCE) == QUEST_STATUS_REWARDED)
                            me->DespawnOrUnsummon();

                    DespawnTimer = 1000;
                }
                else DespawnTimer -= diff;
            }
    };
};


/*######
## npc_nourished_yak_escort
######*/

class npc_nourished_yak_escort : public CreatureScript
{
public:
    npc_nourished_yak_escort(): CreatureScript("npc_nourished_yak_escort") { }

    struct npc_nourished_yak_escortAI : public npc_escortAI
    {
        npc_nourished_yak_escortAI(Creature* creature) : npc_escortAI(creature) {}

        void Reset()
        {
            me->CastSpell(me, 111810, true);
        }

        void WaypointReached(uint32 waypointId)
        {
            Player* player = GetPlayerForEscort();

            switch (waypointId)
            {
                case 1:
                    SetRun();
                    break;
                case 29:
                    if (Creature* chariot = me->FindNearestCreature(57208, 50.00f, true))
                        chariot->DespawnOrUnsummon();

                    me->DespawnOrUnsummon();
                    break;

            }
        }

        void OnCharmed(bool /*apply*/){}

        void UpdateAI(const uint32 uiDiff)
        {
            npc_escortAI::UpdateAI(uiDiff);

            Start(false, true);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_nourished_yak_escortAI(creature);
    }
};

/*######
## npc_delivery_cart_escort
######*/

class npc_delivery_cart_escort : public CreatureScript
{
public:
    npc_delivery_cart_escort(): CreatureScript("npc_delivery_cart_escort") { }

    struct npc_delivery_cart_escortAI : public npc_escortAI
    {
        npc_delivery_cart_escortAI(Creature* creature) : npc_escortAI(creature) {}

        uint32 StartTimer;

        void Reset()
        {
            me->CastSpell(me, 108692, true);
            me->CastSpell(me, 111809, true);
            StartTimer = 800;
        }

        void WaypointReached(uint32 waypointId)
        {
            Player* player = GetPlayerForEscort();

            switch (waypointId)
            {
                case 1:
                    SetRun();
                    break;
                case 29:
                    me->DespawnOrUnsummon();
                    break;

            }
        }

        void OnCharmed(bool /*apply*/){}

        void UpdateAI(const uint32 uiDiff)
        {
            npc_escortAI::UpdateAI(uiDiff);

            if (StartTimer <= uiDiff)
            {
                Start(false, true);
                StartTimer  = 300000;
            }
            else StartTimer -= uiDiff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_delivery_cart_escortAI(creature);
    }
};


/*######
## npc_jojo_ironbrow_plank
######*/

enum JojoIronbrowPlank
{
    SPELL_PLANK_CAST    = 129293,
    SAY_JOJO_PLANK_1    = 0,
    SAY_JOJO_PLANK_2    = 1
};

class npc_jojo_ironbrow_plank : public CreatureScript
{
public:
    npc_jojo_ironbrow_plank(): CreatureScript("npc_jojo_ironbrow_plank") { }

    struct npc_jojo_ironbrow_plankAI : public npc_escortAI
    {
        npc_jojo_ironbrow_plankAI(Creature* creature) : npc_escortAI(creature) {}

        void Reset()
        {
        }

        void WaypointReached(uint32 waypointId)
        {
            Player* player = GetPlayerForEscort();

            switch (waypointId)
            {
                case 1:
                    Talk(SAY_JOJO_PLANK_1);
                    break;
                case 2:
                    me->CastSpell(me, SPELL_PLANK_CAST, true);
                    break;
                case 3:
                    Talk(SAY_JOJO_PLANK_2);
                    break;
                case 12:
                    me->DespawnOrUnsummon();
                    break;
            }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            npc_escortAI::UpdateAI(uiDiff);

            if (UpdateVictim())
                return;

            Start(false, true);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_jojo_ironbrow_plankAI(creature);
    }
};

#define GOSSIP_SHU      "Shu, pouvez-vous réveiller Wugou pour moi ?"

enum NotInTheFaceQuest
{
    QUEST_NOT_IN_THE_FACE           = 29774,
    SPELL_SUMMON_SPIRIT_OF_WATER    = 104017
};

class npc_shu_quest_29774 : public CreatureScript
{
public :
    npc_shu_quest_29774() : CreatureScript("npc_shu_quest_29774"){}

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF+1)
        {
            player->CastSpell(player, SPELL_SUMMON_SPIRIT_OF_WATER, true);
            player->CLOSE_GOSSIP_MENU();
        }
        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetQuestStatus(QUEST_NOT_IN_THE_FACE) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SHU, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        player->SEND_GOSSIP_MENU(55556, creature->GetGUID());
        return true;
    }
};


enum ShuWougou
{
    SPELL_SHUS_WATER_SPLASH = 118027,
    SPELL_WATER_CREDIT      = 104023,
    SPELL_SLEEP             = 52742,

    QUEST_THE_SPIRIT_AND_BODY   = 29775
};

class npc_shu_escort_wugou: public CreatureScript
{
public:
    npc_shu_escort_wugou() : CreatureScript("npc_shu_escort_wugou") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shu_escort_wugouAI(creature);
    }

    struct npc_shu_escort_wugouAI : public npc_escortAI
    {
            npc_shu_escort_wugouAI(Creature* creature) : npc_escortAI(creature) {}

            uint32 DespawnTimer;

            void Reset()
            {
                DespawnTimer = 1000;
            }

            void WaypointReached(uint32 waypointId)
            {
                Player* player = GetPlayerForEscort();

                switch (waypointId)
                {
                    case 7:
                        DoCast(SPELL_SHUS_WATER_SPLASH);
                        break;
                    case 8:
                        SetEscortPaused(true);

                        if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                            me->GetMotionMaster()->MoveFollow(summoner, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
                        break;
                }
            }

            void UpdateAI(const uint32 uiDiff)
            {
                npc_escortAI::UpdateAI(uiDiff);

                if (UpdateVictim())
                    return;

                Start(false, true);

                if (DespawnTimer <= uiDiff)
                {
                    if(Unit* owner = me->GetOwner())
                        if(owner->ToPlayer() && owner->ToPlayer()->GetQuestStatus(QUEST_THE_SPIRIT_AND_BODY) == QUEST_STATUS_REWARDED)
                            me->DespawnOrUnsummon();

                    DespawnTimer = 1000;
                }
                else DespawnTimer -= uiDiff;
            }
    };
};

class npc_wugou_escort: public CreatureScript
{
public:
    npc_wugou_escort() : CreatureScript("npc_wugou_escort") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wugou_escortAI(creature);
    }

    struct npc_wugou_escortAI : public ScriptedAI
    {
            npc_wugou_escortAI(Creature* creature) : ScriptedAI(creature) {}

            uint32 DespawnTimer;

            void Reset()
            {
                me->CastSpell(me, SPELL_SLEEP, true);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                me->SetStandState(UNIT_STAND_STATE_SIT);
                DespawnTimer = 1000;
            }

            void SpellHit(Unit* caster, const SpellInfo* spell)
            {
                if (spell->Id == SPELL_SHUS_WATER_SPLASH)
                {
                    me->RemoveAurasDueToSpell(SPELL_SLEEP);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                    me->SetStandState(UNIT_STAND_STATE_STAND);

                    if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                    {
                        me->GetMotionMaster()->MoveFollow(summoner, PET_FOLLOW_DIST, M_PI);
                        me->CastSpell(summoner, SPELL_WATER_CREDIT, true);
                    }
                }
            }

            void UpdateAI(const uint32 uiDiff)
            {
                if (DespawnTimer <= uiDiff)
                {
                    if(Unit* owner = me->GetOwner())
                        if(owner->ToPlayer() && owner->ToPlayer()->GetQuestStatus(QUEST_THE_SPIRIT_AND_BODY) == QUEST_STATUS_REWARDED)
                            me->DespawnOrUnsummon();

                    DespawnTimer = 1000;
                }
                else DespawnTimer -= uiDiff;
            }
    };
};


/*######
## npc_nourished_yak_escort_2
######*/

class npc_nourished_yak_escort_2 : public CreatureScript
{
public:
    npc_nourished_yak_escort_2(): CreatureScript("npc_nourished_yak_escort_2") { }

    struct npc_nourished_yak_escort_2AI : public npc_escortAI
    {
        npc_nourished_yak_escort_2AI(Creature* creature) : npc_escortAI(creature) {}

        void Reset()
        {
            me->CastSpell(me, 111810, true);
        }

        void WaypointReached(uint32 waypointId)
        {
            Player* player = GetPlayerForEscort();

            switch (waypointId)
            {
                case 1:
                    SetRun();
                    break;
                case 31:
                    if (Creature* chariot = me->FindNearestCreature(59496, 50.00f, true))
                        chariot->DespawnOrUnsummon();

                    me->DespawnOrUnsummon();
                    break;

            }
        }

        void OnCharmed(bool /*apply*/){}

        void UpdateAI(const uint32 uiDiff)
        {
            npc_escortAI::UpdateAI(uiDiff);

            Start(false, true);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_nourished_yak_escort_2AI(creature);
    }
};

/*######
## npc_delivery_cart_escort_2
######*/

class npc_delivery_cart_escort_2 : public CreatureScript
{
public:
    npc_delivery_cart_escort_2(): CreatureScript("npc_delivery_cart_escort_2") { }

    struct npc_delivery_cart_escort_2AI : public npc_escortAI
    {
        npc_delivery_cart_escort_2AI(Creature* creature) : npc_escortAI(creature) {}

        uint32 StartTimer;

        void Reset()
        {
            me->CastSpell(me, 108692, true);
            me->CastSpell(me, 111809, true);
            StartTimer = 800;
        }

        void WaypointReached(uint32 waypointId)
        {
            Player* player = GetPlayerForEscort();

            switch (waypointId)
            {
                case 1:
                    SetRun();
                    break;
                case 31:
                    me->DespawnOrUnsummon();
                    break;

            }
        }

        void OnCharmed(bool /*apply*/){}

        void UpdateAI(const uint32 uiDiff)
        {
            npc_escortAI::UpdateAI(uiDiff);

            if (StartTimer <= uiDiff)
            {
                Start(false, true);
                StartTimer  = 300000;
            }
            else StartTimer -= uiDiff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_delivery_cart_escort_2AI(creature);
    }
};


/*######
## npc_uplifting_draft
######*/

class npc_uplifting_draft : public CreatureScript
{
public:
    npc_uplifting_draft(): CreatureScript("npc_uplifting_draft") { }

    struct npc_uplifting_draftAI : public npc_escortAI
    {
        npc_uplifting_draftAI(Creature* creature) : npc_escortAI(creature) {}

        void Reset()
        {
            me->CastSpell(me, 86134, true);
        }

        void WaypointReached(uint32 waypointId)
        {
            Player* player = GetPlayerForEscort();

            switch (waypointId)
            {
                case 1:
                    SetRun();
                    break;
                case 13:
                    me->DespawnOrUnsummon();
                    break;

            }
        }

        void OnCharmed(bool /*apply*/){}

        void UpdateAI(const uint32 uiDiff)
        {
            npc_escortAI::UpdateAI(uiDiff);

            Start(false, true);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_uplifting_draftAI(creature);
    }
};


enum MasterShangXi
{
    SAY_MASTER_1    = 1,
    SAY_MASTER_2    = 2,
    SAY_MASTER_3    = 3,
    SAY_MASTER_4    = 4,
    ACTION_SAY      = 1
};

class npc_master_shang_xi_temple: public CreatureScript
{
public:
    npc_master_shang_xi_temple() : CreatureScript("npc_master_shang_xi_temple") { }

    bool OnQuestReward(Player* player, Creature* creature, const Quest* quest, uint32 /*slot*/)
    {
        if (quest->GetQuestId() == 29775)
        {
            player->CastSpell(player, 106667, true);
            creature->AI()->DoAction(ACTION_SAY);
        }
        return true;
    }

    struct npc_master_shang_xi_templeAI : public ScriptedAI
    {
            npc_master_shang_xi_templeAI(Creature* creature) : ScriptedAI(creature) {}

            bool StartEvent2;
            uint32 Talk_1_Timer;
            uint32 Talk_2_Timer;
            uint32 Talk_3_Timer;
            uint32 Talk_4_Timer;

            void Reset()
            {
                StartEvent2 = false;
                Talk_1_Timer = 1000;
                Talk_2_Timer = 11000;
                Talk_3_Timer = 21000;
                Talk_4_Timer = 30000;
            }

            void DoAction(int32 const action)
            {
                switch (action)
                {
                    case ACTION_SAY:
                        StartEvent2 = true;
                        break;
                }
            }

            void UpdateAI(const uint32 uiDiff)
            {
                if(StartEvent2)
                {
                    if (Talk_1_Timer <= uiDiff)
                    {
                        Talk(SAY_MASTER_1);
                        Talk_1_Timer = 50000;
                    }
                    else Talk_1_Timer -= uiDiff;

                    if (Talk_2_Timer <= uiDiff)
                    {
                        Talk(SAY_MASTER_2);
                        Talk_2_Timer = 50000;
                    }
                    else Talk_2_Timer -= uiDiff;

                    if (Talk_3_Timer <= uiDiff)
                    {
                        Talk(SAY_MASTER_3);
                        Talk_3_Timer = 50000;
                    }
                    else Talk_3_Timer -= uiDiff;

                    if (Talk_4_Timer <= uiDiff)
                    {
                        Talk(SAY_MASTER_4);
                        Talk_4_Timer = 50000;
                        Reset();
                        StartEvent2 = false;
                    }
                    else Talk_4_Timer -= uiDiff;
                }
            }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_master_shang_xi_templeAI(creature);
    }
};


/*######
## npc_ji_firepaw_escort
######*/

enum JiFirepaw
{
    SAY_FIREPAW    = 0
};


class npc_ji_firepaw_escort : public CreatureScript
{
public:
    npc_ji_firepaw_escort(): CreatureScript("npc_ji_firepaw_escort") { }

    struct npc_ji_firepaw_escortAI : public npc_escortAI
    {
        npc_ji_firepaw_escortAI(Creature* creature) : npc_escortAI(creature) {}

        void Reset()
        {
        }

        void WaypointReached(uint32 waypointId)
        {
            Player* player = GetPlayerForEscort();

            switch (waypointId)
            {
                case 1:
                    SetRun();
                    break;
                case 2:
                    Talk(SAY_FIREPAW);
                    break;
                case 73:
                    me->DespawnOrUnsummon();
                    break;

            }
        }

        void OnCharmed(bool /*apply*/){}

        void UpdateAI(const uint32 uiDiff)
        {
            npc_escortAI::UpdateAI(uiDiff);

            Start(false, true);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ji_firepaw_escortAI(creature);
    }
};

/*#####
## at_pop_zhao_ren
#####*/

enum PopZhaoRen
{
    NPC_ZHAO_REN_POP                = 64554,
    QUEST_MORMING_BREEZE_VILLAGE    = 29776
};

class at_pop_zhao_ren : public AreaTriggerScript
{
public :
    at_pop_zhao_ren() : AreaTriggerScript("at_pop_zhao_ren") {}

    bool OnTrigger(Player *player, const AreaTriggerEntry *at)
    {
        std::map<uint64, uint32>::iterator iter = forbiddenPlayers.find(player->GetGUID());
        if(iter != forbiddenPlayers.end())
            return false;
        if (player->GetQuestStatus(QUEST_MORMING_BREEZE_VILLAGE) == QUEST_STATUS_COMPLETE)
        {
            forbiddenPlayers.insert(std::pair<uint64, uint32>(player->GetGUID(), 60000));
            player->SummonCreature(NPC_ZHAO_REN_POP, 823.51f, 3941.58f, 273.00f, 4.89f);
            return true;
        }
        return false;
    }

    void Update(const uint32 uiDiff)
    {
        for (std::map<uint64, uint32>::iterator iter = forbiddenPlayers.begin() ; iter != forbiddenPlayers.end() ; ++iter)
        {
            if(iter->second <= uiDiff)
                forbiddenPlayers.erase(iter);
            else
                iter->second -= uiDiff ;
        }
    }

private :
    std::map<uint64, uint32> forbiddenPlayers ;
};

enum Area8287
{
    NPC_ZAN     = 64885,
    SAY_ZAN     = 0
};

class at_zan_talk : public AreaTriggerScript
{
public :
    at_zan_talk() : AreaTriggerScript("at_zan_talk") {}

    bool OnTrigger(Player *player, const AreaTriggerEntry *at)
    {
        std::map<uint64, uint32>::iterator iter = forbiddenPlayers.find(player->GetGUID());
        if(iter != forbiddenPlayers.end())
            return false;
        if(player->GetQuestStatus(QUEST_MORMING_BREEZE_VILLAGE) == QUEST_STATUS_COMPLETE)
        {
            Creature* zan = player->FindNearestCreature(NPC_ZAN, 500.0f);
            if(zan)
            {
                forbiddenPlayers.insert(std::pair<uint64, uint32>(player->GetGUID(), 120000));
                zan->AI()->Talk(SAY_ZAN, player->GetGUID());
                return true;
            }
        }
        return false;
    }

    void Update(const uint32 uiDiff)
    {
        for (std::map<uint64, uint32>::iterator iter = forbiddenPlayers.begin() ; iter != forbiddenPlayers.end() ; ++iter)
        {
            if(iter->second <= uiDiff)
                forbiddenPlayers.erase(iter);
            else
                iter->second -= uiDiff ;
        }
    }

private :
    std::map<uint64, uint32> forbiddenPlayers ;
};

/*######
## npc_zhao_ren_pop
######*/

class npc_zhao_ren_pop : public CreatureScript
{
public:
    npc_zhao_ren_pop(): CreatureScript("npc_zhao_ren_pop") { }

    struct npc_zhao_ren_popAI : public npc_escortAI
    {
        npc_zhao_ren_popAI(Creature* creature) : npc_escortAI(creature) {}

        void Reset()
        {
            me->setActive(true);
        }

        void WaypointReached(uint32 waypointId)
        {
            Player* player = GetPlayerForEscort();

            switch (waypointId)
            {
                case 1:
                    SetRun();
                    me->SetSpeed(MOVE_FLIGHT, 3.0f);
                    break;
                case 11:
                    me->DespawnOrUnsummon();
                    break;

            }
        }

        void OnCharmed(bool /*apply*/){}

        void UpdateAI(const uint32 uiDiff)
        {
            npc_escortAI::UpdateAI(uiDiff);

            Start(false, true);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_zhao_ren_popAI(creature);
    }
};

/*######
## npc_head_shen_zin_su
######*/

class npc_head_shen_zin_su : public CreatureScript
{
public:
    npc_head_shen_zin_su(): CreatureScript("npc_head_shen_zin_su") { }

    struct npc_head_shen_zin_suAI : public ScriptedAI
    {
        npc_head_shen_zin_suAI(Creature* creature) : ScriptedAI(creature) {}

        void Reset()
        {
            me->setActive(true);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        }

        void UpdateAI(const uint32 uiDiff)
        {
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_head_shen_zin_suAI(creature);
    }
};

/*######
## npc_jojo_ironbrow_stack
######*/

enum JojoIronbrowStack
{
    SPELL_STACK_CAST    = 129294,
    SAY_JOJO_STACK_1    = 0,
    SAY_JOJO_STACK_2    = 1
};

class npc_jojo_ironbrow_stack : public CreatureScript
{
public:
    npc_jojo_ironbrow_stack(): CreatureScript("npc_jojo_ironbrow_stack") { }

    struct npc_jojo_ironbrow_stackAI : public npc_escortAI
    {
        npc_jojo_ironbrow_stackAI(Creature* creature) : npc_escortAI(creature) {}

        void Reset()
        {
        }

        void WaypointReached(uint32 waypointId)
        {
            Player* player = GetPlayerForEscort();

            switch (waypointId)
            {
                case 1:
                    Talk(SAY_JOJO_STACK_1);
                    break;
                case 2:
                    me->CastSpell(me, SPELL_STACK_CAST, true);
                    break;
                case 3:
                    Talk(SAY_JOJO_STACK_2);
                    break;
                case 4:
                    me->DespawnOrUnsummon();
                    break;
            }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            npc_escortAI::UpdateAI(uiDiff);

            if (UpdateVictim())
                return;

            Start(false, true);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_jojo_ironbrow_stackAI(creature);
    }
};


/*######
## npc_jojo_ironbrow_pillar
######*/

enum JojoIronbrowPillar
{
    SPELL_PILLAR_CAST       = 129297,
    SAY_JOJO_PILLAR_1       = 0,
    SAY_JOJO_PILLAR_2       = 1,
    SAY_JOJO_PILLAR_3       = 2,
    SAY_JOJO_PILLAR_4       = 3
};

class npc_jojo_ironbrow_pillar : public CreatureScript
{
public:
    npc_jojo_ironbrow_pillar(): CreatureScript("npc_jojo_ironbrow_pillar") { }

    struct npc_jojo_ironbrow_pillarAI : public npc_escortAI
    {
        npc_jojo_ironbrow_pillarAI(Creature* creature) : npc_escortAI(creature) {}

        void Reset()
        {
        }

        void WaypointReached(uint32 waypointId)
        {
            Player* player = GetPlayerForEscort();

            switch (waypointId)
            {
                case 1:
                    Talk(SAY_JOJO_PILLAR_1);
                    break;
                case 2:
                    Talk(SAY_JOJO_PILLAR_2);
                    break;
                case 3:
                    me->CastSpell(me, SPELL_PILLAR_CAST, true);
                    break;
                case 4:
                    Talk(SAY_JOJO_PILLAR_3);
                    break;
                case 5:
                    Talk(SAY_JOJO_PILLAR_4);
                    me->SetStandState(UNIT_STAND_STATE_SLEEP);
                    break;
                case 6:
                    me->DespawnOrUnsummon();
                    break;
            }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            npc_escortAI::UpdateAI(uiDiff);

            if (UpdateVictim())
                return;

            Start(false, true);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_jojo_ironbrow_pillarAI(creature);
    }
};

class spell_shoot_all_the_fireworks_periodic : public SpellScriptLoader
{
public :
    spell_shoot_all_the_fireworks_periodic() : SpellScriptLoader("spell_shoot_all_the_fireworks")
    {

    }

    class spell_shoot_all_the_fireworks_periodic_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_shoot_all_the_fireworks_periodic_SpellScript)

        bool Validate(const SpellInfo *spellInfo)
        {
            if(!sSpellMgr->GetSpellInfo(109103))
                return false;

            return true ;
        }

        bool Load()
        {
            return GetCaster() != NULL;
        }

        void SetTarget(WorldObject*& target)
        {
            if(Unit* caster = GetCaster())
            {
                if(caster->getVictim())
                    target = caster->getVictim();
                else
                    target = NULL ;
            }
        }

        void Register()
        {
            OnObjectTargetSelect += SpellObjectTargetSelectFn(spell_shoot_all_the_fireworks_periodic_SpellScript::SetTarget, EFFECT_0, TARGET_UNIT_TARGET_ANY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_shoot_all_the_fireworks_periodic_SpellScript();
    }
};

/*######
## npc_huojin_monk_escort
######*/

enum HuojinMonk
{
    SPELL_JAB_HUOJIN            = 128630,
    SPELL_BLACKOUT_KICK_HUOJIN  = 128631,
    SPELL_CRANE_KICK            = 128632,

    SAY_HUOJIN_MONK_1           = 0,
    SAY_HUOJIN_MONK_2           = 1,
    SAY_HUOJIN_MONK_3           = 2,
    SAY_HUOJIN_MONK_4           = 3
};

class npc_huojin_monk_escort : public CreatureScript
{
public:
    npc_huojin_monk_escort(): CreatureScript("npc_huojin_monk_escort") { }

    struct npc_huojin_monk_escortAI : public ScriptedAI
    {
        npc_huojin_monk_escortAI(Creature* creature) : ScriptedAI(creature)
        {
            SayIntro = true;
        }

        bool SayIntro;

        uint32 TestAreaTimer;
        uint32 CombatTimer;
        uint32 JabTimer;
        uint32 BlackoutTimer;
        uint32 CraneTimer;

        void Reset()
        {
            me->SetReactState(REACT_PASSIVE);
            me->SetHealth(me->GetMaxHealth());
            TestAreaTimer = 5000;
            CombatTimer = 1000;
            JabTimer = 2000;
            BlackoutTimer = 4000;
            CraneTimer = 6000;
        }

        void DamageTaken(Unit* attacker, uint32 &amount)
        {
            me->SetReactState(REACT_AGGRESSIVE);
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if(SayIntro)
            {
                Talk(SAY_HUOJIN_MONK_1);
                SayIntro = false;
            }

            if (TestAreaTimer <= uiDiff)
            {
                if(me->GetAreaId() != 5831)
                    me->DespawnOrUnsummon();

                TestAreaTimer = 1000;
            }
            else TestAreaTimer -= uiDiff;

            if (CombatTimer <= uiDiff)
            {
                if(Unit* summoner = me->ToTempSummon()->GetSummoner())
                {
                    if(summoner->ToPlayer())
                    {
                        if(!summoner->isInCombat())
                            me->SetReactState(REACT_PASSIVE);
                        if(summoner->isInCombat())
                            me->SetReactState(REACT_AGGRESSIVE);
                    }
                }
                CombatTimer = 1000;
            }
            else CombatTimer -= uiDiff;

            if (!UpdateVictim())
                return;

            if (JabTimer <= uiDiff)
            {
                me->CastSpell(me->getVictim(), SPELL_JAB_HUOJIN, false);
                JabTimer = 12000;
            }
            else JabTimer -= uiDiff;

            if (BlackoutTimer <= uiDiff)
            {
                me->CastSpell(me->getVictim(), SPELL_BLACKOUT_KICK_HUOJIN, false);
                BlackoutTimer = 12000;
            }
            else BlackoutTimer -= uiDiff;

            if (CraneTimer <= uiDiff)
            {
                me->CastSpell(me, SPELL_CRANE_KICK, false);
                CraneTimer = 12000;
            }
            else CraneTimer -= uiDiff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_huojin_monk_escortAI(creature);
    }
};

/*######
## npc_ruk_ruk
######*/

enum RukRuk
{
};

class npc_ruk_ruk : public CreatureScript
{
public:
    npc_ruk_ruk(): CreatureScript("npc_ruk_ruk") { }

    struct npc_ruk_rukAI : public ScriptedAI
    {
        npc_ruk_rukAI(Creature* creature) : ScriptedAI(creature){}

        void Reset()
        {
            m_uiRocketTimer = 10000;
            m_uiOuksplosionsTimer = 5000;
            Ruk50 = true;
            Ruk25 = true;
        }

        void DamageTaken(Unit* attacker, uint32 &amount)
        {
            if(Creature* huojin = me->FindNearestCreature(65558, 40.0f, true))
            {
                if(me->GetHealthPct() <= 50 && Ruk50)
                {
                    huojin->AI()->Talk(SAY_HUOJIN_MONK_2);
                    Ruk50 = false;
                }

                if(me->GetHealthPct() <= 25 && Ruk25)
                {
                    huojin->AI()->Talk(SAY_HUOJIN_MONK_3);
                    Ruk25 = false;
                }
            }
        }

        void JustDied(Unit* /*killer*/)
        {
            if(Creature* huojin = me->FindNearestCreature(65558, 40.0f, true))
                huojin->AI()->Talk(SAY_HUOJIN_MONK_4);
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if(!UpdateVictim())
                return ;
            
            if(m_uiRocketTimer <= uiDiff)
            {
                if(Unit* unit = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f, true))
                {
                    Position pos ; unit->GetPosition(&pos);
                    if(Creature* summon = me->SummonCreature(64322, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN))
                    {
                        summon->GetMotionMaster()->MovePoint(0, pos);
                        summon->AI()->SetData(0, 1);
                    }
                }
                m_uiRocketTimer = 10000;
            }
            else
                m_uiRocketTimer -= uiDiff;

            if(m_uiOuksplosionsTimer <= uiDiff)
            {
                if(Unit* unit = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f, true))
                    me->CastSpell(unit, 125699, false);

                m_uiOuksplosionsTimer = 10000;
            }
            else
                m_uiOuksplosionsTimer -= uiDiff;
            
            DoMeleeAttackIfReady();
        }
        
    private :
        uint32 m_uiRocketTimer;
        uint32 m_uiOuksplosionsTimer;
        bool Ruk50;
        bool Ruk25;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ruk_rukAI(creature);
    }
};


class mob_ruk_ruk_rocket : public CreatureScript
{
public :
    mob_ruk_ruk_rocket() : CreatureScript("mob_ruk_ruk_rocket")
    {      
    }
    
    class mob_ruk_ruk_rocket_AIScript : public ScriptedAI
    {
    public :
        mob_ruk_ruk_rocket_AIScript(Creature* creature) : ScriptedAI(creature)
        {
            initialized = false;
            m_uiCheckTimer = 500;
            me->CastSpell(me, 125613, true);
        }
        
        void SetData(uint32 index, uint32 value)
        {
            if(index == 0)
                initialized = true;
        }
        
        void MovementInform(uint32 motionType, uint32 id)
        {
            if(motionType == POINT_MOTION_TYPE && id == 0)
            {
                DoCastAOE(125619, true);
                me->DisappearAndDie();
            }
        }
        
        void UpdateAI(const uint32 diff)
        {
            if(!initialized)
                return;
            
            if(m_uiCheckTimer <= diff)
            {
                CheckForPlayers();
                m_uiCheckTimer = 500;
            }
            else
                m_uiCheckTimer -= diff;
        }
        
        void CheckForPlayers()
        {
            Trinity::AllWorldObjectsInRange u_check(me, 2.0f);
            std::list<Player*> players ;
            Trinity::PlayerListSearcher<Trinity::AllWorldObjectsInRange> searcher(me, players, u_check);
            me->VisitNearbyObject(2.0f, searcher);
            
            for(std::list<Player*>::iterator iter = players.begin() ; iter != players.end() ; ++iter)
            {
                if(Player* player = *iter)
                    DoCast(player, 125612, true);
            }
        }
        
    private :
        bool initialized;
        uint32 m_uiCheckTimer;
    };
    
    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_ruk_ruk_rocket_AIScript(creature);
    }
};

class spell_ruk_ruk_ooksplosions : public SpellScriptLoader
{
public :
    spell_ruk_ruk_ooksplosions() : SpellScriptLoader("spell_ruk_ruk_ooksplosions")
    {
        
    }
    
    class spell_ruk_ruk_ooksplosions_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_ruk_ruk_ooksplosions_AuraScript)
        
        bool Validate(const SpellInfo* /*spellInfo*/)
        {
            if(sSpellMgr->GetSpellInfo(125699)
                    && sSpellMgr->GetSpellInfo(125885)
                    && sSpellMgr->GetSpellInfo(125887))
                return true;
            
            return false;
        }
        
        bool Load()
        {
            return true;
        }
        
        void HandlePeriodicDummyTick(AuraEffect const* auraEff)
        {
            if(GetCaster())
                GetCaster()->CastSpell(GetCaster(), GetSpellInfo()->Effects[0].BasePoints, true);
        }
        
        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_ruk_ruk_ooksplosions_AuraScript::HandlePeriodicDummyTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        }
    };
    
    AuraScript* GetAuraScript() const
    {
        return new spell_ruk_ruk_ooksplosions_AuraScript();
    }
};

/*#####
## at_wind_chamber
#####*/

enum eWindChamber
{
    QUEST_DAFENG_AIR            = 29785,
    SPELL_SUMMON_AYSA_CHAMBER   = 104571,

    SAY_AYSA_WIND_CHAMBER_1     = 0,
    SAY_AYSA_WIND_CHAMBER_2     = 1,
    SAY_AYSA_WIND_CHAMBER_3     = 2,
    SAY_AYSA_WIND_CHAMBER_4     = 3
};

class at_wind_chamber : public AreaTriggerScript
{
    public:

        at_wind_chamber(): AreaTriggerScript("at_wind_chamber")
        {
        }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/)
        {
            if (player->isAlive())
            {
                if (player->GetQuestStatus(QUEST_DAFENG_AIR) == QUEST_STATUS_INCOMPLETE)
                {
                    if(!player->HasAura(SPELL_SUMMON_AYSA_CHAMBER))
                    {
                        player->CastSpell(player, SPELL_SUMMON_AYSA_CHAMBER, true);
                        return true;
                    }
                }
            }
            return false;
        }
};

class npc_aysa_wind_escort : public CreatureScript
{
public:
    npc_aysa_wind_escort(): CreatureScript("npc_aysa_wind_escort") { }

    struct npc_aysa_wind_escortAI : public npc_escortAI
    {
        npc_aysa_wind_escortAI(Creature* creature) : npc_escortAI(creature) {}

        bool VerifPlayer;

        void Reset()
        {
            VerifPlayer = false;
            Talk(SAY_AYSA_WIND_CHAMBER_1);
        }

        void WaypointReached(uint32 waypointId)
        {
            Player* player = GetPlayerForEscort();

            switch (waypointId)
            {
                case 1:
                    Talk(SAY_AYSA_WIND_CHAMBER_2);
                    SetEscortPaused(true);
                    VerifPlayer = true;
                    break;
                case 5:
                    SetEscortPaused(true);
                    VerifPlayer = true;
                    break;
                case 6:
                    Talk(SAY_AYSA_WIND_CHAMBER_3);
                    break;
                case 10:
                    SetEscortPaused(true);
                    VerifPlayer = true;
                    break;
                case 11:
                    Talk(SAY_AYSA_WIND_CHAMBER_4);
                    if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                        if(summoner->ToPlayer())
                            summoner->ToPlayer()->KilledMonsterCredit(55666);
                    break;
                case 12:
                    me->DespawnOrUnsummon();
                    break;
            }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            npc_escortAI::UpdateAI(uiDiff);

            if (VerifPlayer)
            {
                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                    if(summoner->ToPlayer())
                        if(summoner->IsInDist2d(me, 3.00f))
                        {
                            SetEscortPaused(false);
                            VerifPlayer = false;
                        }
            }

            Start(false, true);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_aysa_wind_escortAI(creature);
    }
};

enum eZhaoRen
{
    SAY_AYSA_ZHAO_1     = 0,
    SAY_AYSA_ZHAO_2     = 1,
    SAY_AYSA_ZHAO_3     = 2
};

class npc_zhao_ren : public CreatureScript
{
public:
    npc_zhao_ren(): CreatureScript("npc_zhao_ren") { }

    struct npc_zhao_renAI : public npc_escortAI
    {
        npc_zhao_renAI(Creature* creature) : npc_escortAI(creature) {}

        bool VerifDamage;
        bool VerifHP80;
        bool VerifHP70;
        bool VerifHP20;
        bool Descente;
        bool Attaque;

        uint32 DescenteTimer;
        uint32 RemonteTimer;
        uint32 CastTimer;

        void Reset()
        {
            VerifDamage = false;
            VerifHP80 = true;
            VerifHP70 = true;
            VerifHP20 = true;
            Descente = false;
            Attaque = false;
            DescenteTimer = 12000;
            RemonteTimer = 15000;
            CastTimer = 6000;
            me->InterruptNonMeleeSpells(false);
            me->SetReactState(REACT_PASSIVE);
            me->SetCanFly(true);
            me->AttackStop();
        }

        void DamageTaken(Unit* caster, uint32 &damage)
        {
            if(damage >= 1 && !VerifDamage)
            {
                VerifDamage = true;
            } 
        }

        void WaypointReached(uint32 waypointId)
        {
            Player* player = GetPlayerForEscort();
        }

        void UpdateAI(const uint32 uiDiff)
        {
            npc_escortAI::UpdateAI(uiDiff);

            if(me->GetHealthPct() <= 80 && VerifHP80)
            {
                if(Creature* aysa = me->FindNearestCreature(64506, 200.00f, true))
                    aysa->AI()->Talk(SAY_AYSA_ZHAO_1);

                VerifHP80 = false;
            }
            if(me->GetHealthPct() <= 70 && VerifHP70)
            {
                if(Creature* aysa = me->FindNearestCreature(64506, 200.00f, true))
                    aysa->AI()->Talk(SAY_AYSA_ZHAO_2);

                me->GetMotionMaster()->MovePoint(7, 723.16f, 4163.79f, 196.083f);
                SetEscortPaused(true);
                Descente = true;
                VerifHP70 = false;
            }

            if(me->GetHealthPct() <= 20 && VerifHP20)
            {
                if(Creature* aysa = me->FindNearestCreature(64506, 200.00f, true))
                    aysa->AI()->Talk(SAY_AYSA_ZHAO_3);

                me->GetMotionMaster()->MovePoint(7, 723.16f, 4163.79f, 196.083f);
                SetEscortPaused(true);
                Attaque = true;
                VerifHP20 = false;
            }

            if(VerifDamage)
                Start(false, true, 0, 0, true, true, true);

            if (Descente)
            {
                if(DescenteTimer <= uiDiff)
                {
                    me->CastSpell(me, 125990, false);
                    DescenteTimer = 15000;
                }
                else
                    DescenteTimer -= uiDiff;

                if(RemonteTimer <= uiDiff)
                {
                    float fRetX, fRetY, fRetZ;
                    me->GetRespawnPosition(fRetX, fRetY, fRetZ);
                    me->GetMotionMaster()->MovePoint(0xFFFFFE, fRetX, fRetY, fRetZ);
                    SetEscortPaused(false);
                    RemonteTimer = 15000;
                    Descente = false;
                }
                else
                    RemonteTimer -= uiDiff;
            }

            if(!Descente)
            {
                if(CastTimer <= uiDiff)
                {
                    if(Unit* unit = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                        if(unit->ToPlayer())
                            me->CastSpell(unit, 126006, false);

                    CastTimer = 6000;
                }
                else
                    CastTimer -= uiDiff;
            }

            if(Attaque)
            {
                me->SetReactState(REACT_AGGRESSIVE);
                Attaque = false;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_zhao_renAI(creature);
    }
};

enum eFireworkLauncher
{
    SPELL_LAUNCHER_INACTIVE = 125964
};

class npc_firework_launcher : public CreatureScript
{
public:
    npc_firework_launcher(): CreatureScript("npc_firework_launcher") { }

    struct npc_firework_launcherAI : public ScriptedAI
    {
        npc_firework_launcherAI(Creature* creature) : ScriptedAI(creature){}

        uint32 VerifAuraTimer;

        void Reset()
        {
            me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
            VerifAuraTimer = 100;
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if(VerifAuraTimer <= uiDiff)
            {
                if(me->HasAura(SPELL_LAUNCHER_INACTIVE))
                    me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                else
                    me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);

                VerifAuraTimer = 100;
            }
            else
                VerifAuraTimer -= uiDiff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_firework_launcherAI(creature);
    }
};


class npc_ji_firepaw_zhao : public CreatureScript
{
public:
    npc_ji_firepaw_zhao(): CreatureScript("npc_ji_firepaw_zhao") { }

    struct npc_ji_firepaw_zhaoAI : public ScriptedAI
    {
        npc_ji_firepaw_zhaoAI(Creature* creature) : ScriptedAI(creature)
        {
            launcher = NULL;
        }

        Creature* launcher;

        bool VerifLauncher;
        bool LauncherRepair;

        uint32 VerifLauncherTimer;
        uint32 LauncherRepairTimer;

        void Reset()
        {
            VerifLauncher = true;
            LauncherRepair = false;
            VerifLauncherTimer = 500;
            LauncherRepairTimer = 1000;
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if(VerifLauncher)
            {
                if(VerifLauncherTimer <= uiDiff)
                {
                    std::list<Creature*> launchers;
                    GetCreatureListWithEntryInGrid(launchers, me, 64507, 500.0f);

                    std::list<Creature*>::const_iterator iter = launchers.begin();
                    std::advance(iter, launchers.size()-1);
                    launcher = *iter ;

                    if(launcher->HasAura(SPELL_LAUNCHER_INACTIVE))
                    {
                        Position pos;
                        launcher->GetPosition(&pos);
                        me->GetMotionMaster()->MovePoint(0, pos);
                        LauncherRepair = true;
                        VerifLauncher = false;
                    }

                    VerifLauncherTimer = 500;
                }
                else
                    VerifLauncherTimer -= uiDiff;
            }

            if(LauncherRepair)
            {
                if(LauncherRepairTimer <= uiDiff)
                {
                    if(Creature* launcher = me->FindNearestCreature(64507, 2.00f, true))
                        if(launcher->HasAura(SPELL_LAUNCHER_INACTIVE))
                        {
                            launcher->RemoveAurasDueToSpell(SPELL_LAUNCHER_INACTIVE);
                            VerifLauncher = true;
                            LauncherRepair = false;
                        }

                    LauncherRepairTimer = 1000;
                }
                else
                    LauncherRepairTimer -= uiDiff;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ji_firepaw_zhaoAI(creature);
    }
};

class npc_aysa_wind_exit_escort : public CreatureScript
{
public:
    npc_aysa_wind_exit_escort(): CreatureScript("npc_aysa_wind_exit_escort") { }

    struct npc_aysa_wind_exit_escortAI : public npc_escortAI
    {
        npc_aysa_wind_exit_escortAI(Creature* creature) : npc_escortAI(creature) {}

        void Reset()
        {
        }

        void WaypointReached(uint32 waypointId)
        {
            Player* player = GetPlayerForEscort();

            switch (waypointId)
            {
                case 1:
                    SetRun();
                    break;
                case 8:
                    me->GetMotionMaster()->MoveJump(674.88f, 4202.82f, 197.00f, 20, 20);
                    break;
                case 9:
                    me->DespawnOrUnsummon();
                    break;
            }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            npc_escortAI::UpdateAI(uiDiff);

            Start(false, true);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_aysa_wind_exit_escortAI(creature);
    }
};

class npc_dafeng_escort : public CreatureScript
{
public:
    npc_dafeng_escort(): CreatureScript("npc_dafeng_escort") { }

    struct npc_dafeng_escortAI : public npc_escortAI
    {
        npc_dafeng_escortAI(Creature* creature) : npc_escortAI(creature) {}

        void Reset()
        {
        }

        void WaypointReached(uint32 waypointId)
        {
            Player* player = GetPlayerForEscort();

            switch (waypointId)
            {
                case 1:
                    SetRun();
                    break;
                case 10:
                    me->DespawnOrUnsummon();
                    break;
            }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            npc_escortAI::UpdateAI(uiDiff);

            Start(false, true);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dafeng_escortAI(creature);
    }
};

enum eMasterShangXiEscort
{
    QUEST_WORTHY_OF_PASSING     = 29787,

    SAY_MASTER_ESCORT_1         = 0,
    SAY_MASTER_ESCORT_2         = 1,
    SAY_MASTER_ESCORT_3         = 2,
    SAY_MASTER_ESCORT_4         = 3,
    SAY_MASTER_ESCORT_5         = 4,
    SAY_MASTER_ESCORT_6         = 5
};

class npc_master_shang_xi_escort : public CreatureScript
{
public:
    npc_master_shang_xi_escort(): CreatureScript("npc_master_shang_xi_escort") { }

    struct npc_master_shang_xi_escortAI : public npc_escortAI
    {
        npc_master_shang_xi_escortAI(Creature* creature) : npc_escortAI(creature) {}

        bool VerifPlayer;

        void Reset()
        {
            VerifPlayer = false;
            me->SetReactState(REACT_PASSIVE);
        }

        void WaypointReached(uint32 waypointId)
        {
            Player* player = GetPlayerForEscort();

            switch (waypointId)
            {
                case 1:
                    SetRun(true);
                    break;
                case 2:
                    Talk(SAY_MASTER_ESCORT_1);
                    break;
                case 21:
                    SetRun(false);
                    Talk(SAY_MASTER_ESCORT_2);
                    break;
                case 24:
                    Talk(SAY_MASTER_ESCORT_3);
                    break;
                case 28:
                    Talk(SAY_MASTER_ESCORT_4);
                    break;
                case 31:
                    SetEscortPaused(true);
                    VerifPlayer = true;
                    break;
                case 32:
                    Talk(SAY_MASTER_ESCORT_5);
                    SetRun(true);
                    break;
                case 43:
                    Talk(SAY_MASTER_ESCORT_6);
                    break;
                case 44:
                    me->DespawnOrUnsummon();
                    break;
            }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            npc_escortAI::UpdateAI(uiDiff);

            if (VerifPlayer)
            {
                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                    if(summoner->ToPlayer())
                        if(summoner->ToPlayer()->GetQuestStatus(QUEST_WORTHY_OF_PASSING) == QUEST_STATUS_COMPLETE)
                            if(summoner->IsInDist2d(me, 200.00f))
                            {
                                SetEscortPaused(false);
                                VerifPlayer = false;
                            }
            }

            Start(false, true);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_master_shang_xi_escortAI(creature);
    }
};

enum eMasterShangXiDead
{
    SAY_MASTER_DEAD_1           = 0,
    SAY_MASTER_DEAD_2           = 1,
    SAY_MASTER_DEAD_3           = 2,
    SAY_MASTER_DEAD_4           = 3,
    SAY_MASTER_DEAD_5           = 4,
    SAY_MASTER_DEAD_6           = 5
};

class npc_master_shang_xi_dead : public CreatureScript
{
public:
    npc_master_shang_xi_dead(): CreatureScript("npc_master_shang_xi_dead") { }

    struct npc_master_shang_xi_deadAI : public npc_escortAI
    {
        npc_master_shang_xi_deadAI(Creature* creature) : npc_escortAI(creature) {}

        void Reset()
        {
            me->CastSpell(me, 126160, true);
        }

        void WaypointReached(uint32 waypointId)
        {
            Player* player = GetPlayerForEscort();

            switch (waypointId)
            {
                case 1:
                    Talk(SAY_MASTER_DEAD_1);
                    break;
                case 2:
                    Talk(SAY_MASTER_DEAD_2);
                    break;
                case 3:
                    if(Unit* summoner = me->ToTempSummon()->GetSummoner())
                        if(summoner->ToPlayer())
                            Talk(SAY_MASTER_DEAD_3, summoner->ToPlayer()->GetGUID());
                    break;
                case 6:
                    Talk(SAY_MASTER_DEAD_4);
                    break;
                case 7:
                    Talk(SAY_MASTER_DEAD_5);
                    me->RemoveAurasDueToSpell(126160);
                    me->SummonCreature(57874, 873.21f, 4461.69f, 241.50f, 2.95f, TEMPSUMMON_TIMED_DESPAWN, 60000);
                    break;
                case 8:
                    Talk(SAY_MASTER_DEAD_6);
                    break;
                case 9:
                    me->SetStandState(UNIT_STAND_STATE_KNEEL);
                    me->CastSpell(me, 128851, true);
                    break;
                case 10:
                    if(Unit* summoner = me->ToTempSummon()->GetSummoner())
                        if(summoner->ToPlayer())
                            me->CastSpell(summoner, 106625, true);
                    me->CastSpell(me, 109336, true);
                    me->DespawnOrUnsummon();
                    break;
            }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            npc_escortAI::UpdateAI(uiDiff);

            Start(false, false);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_master_shang_xi_deadAI(creature);
    }
};


/* Event Area 7106 mongolfière */

enum eAysa7106
{
    SAY_AYSA_7106_1     = 0,
    SAY_AYSA_7106_2     = 1
};

enum eJi7106
{
    SAY_JI_7106_1   = 0
};

class npc_aysa_7106 : public CreatureScript
{
public:
    npc_aysa_7106(): CreatureScript("npc_aysa_7106") { }

    struct npc_aysa_7106AI : public ScriptedAI
    {
        npc_aysa_7106AI(Creature* creature) : ScriptedAI(creature){}

        bool Action;

        uint32 Say1_timer;
        uint32 Say2_timer;

        void Reset()
        {
            Action = false;
        }

        void DoAction(int32 const action)
        {
            Action = true;
            Say1_timer = 1000;
            Say2_timer = 14000;
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if(Action)
            {
                if(Say1_timer <= uiDiff)
                {
                    Talk(SAY_AYSA_7106_1);
                    Say1_timer = 60000;
                }
                else
                    Say1_timer -= uiDiff;

                if(Say2_timer <= uiDiff)
                {
                    Talk(SAY_AYSA_7106_2);
                    Say2_timer = 60000;
                    Action = false;
                }
                else
                    Say2_timer -= uiDiff;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_aysa_7106AI(creature);
    }
};

class npc_ji_7106 : public CreatureScript
{
public:
    npc_ji_7106(): CreatureScript("npc_ji_7106") { }

    struct npc_ji_7106AI : public ScriptedAI
    {
        npc_ji_7106AI(Creature* creature) : ScriptedAI(creature){}

        bool Action;

        uint32 Say1_timer;

        void Reset()
        {
            Action = false;
        }

        void DoAction(int32 const action)
        {
            Action = true;
            Say1_timer = 5000;
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if(Action)
            {
                if(Say1_timer <= uiDiff)
                {
                    Talk(SAY_JI_7106_1);
                    Say1_timer = 60000;
                    Action = false;
                }
                else
                    Say1_timer -= uiDiff;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ji_7106AI(creature);
    }
};


/* Event mongolfière (29791) */

enum eShenXiBallon
{
    SAY_SHEN_ZI_BALLON_1    = 0,
    SAY_SHEN_ZI_BALLON_2    = 1,
    SAY_SHEN_ZI_BALLON_3    = 2,
    SAY_SHEN_ZI_BALLON_4    = 3,
    SAY_SHEN_ZI_BALLON_5    = 4,
    SAY_SHEN_ZI_BALLON_6    = 5
};

enum eAysaBallon
{
    SAY_AYSA_BALLON_1       = 0,
    SAY_AYSA_BALLON_2       = 1,
    SAY_AYSA_BALLON_3       = 2,
    SAY_AYSA_BALLON_4       = 3,
    SAY_AYSA_BALLON_5       = 4,
    SAY_AYSA_BALLON_6       = 5,
    SAY_AYSA_BALLON_7       = 6,
    SAY_AYSA_BALLON_8       = 7,
    SAY_AYSA_BALLON_9       = 8,
    SAY_AYSA_BALLON_10      = 9,
    SAY_AYSA_BALLON_11      = 10
};

enum eJiBallon
{
    SAY_JI_BALLON_1         = 0,
    SAY_JI_BALLON_2         = 1,
    SAY_JI_BALLON_3         = 2,
    SAY_JI_BALLON_4         = 3,
    SAY_JI_BALLON_5         = 4,
    SAY_JI_BALLON_6         = 5,
    SAY_JI_BALLON_7         = 6
};

class npc_shang_xi_air_balloon : public VehicleScript
{
public:
    npc_shang_xi_air_balloon(): VehicleScript("npc_shang_xi_air_balloon")
    {
    }

    void OnAddPassenger(Vehicle* veh, Unit* passenger, int8 seatId)
    {
        if(veh->GetBase()->ToCreature()->GetEntry() == 55649)
        {
            Creature* escort = veh->GetBase()->ToCreature()->FindNearestCreature(31002, 200.00f, true);

            if(passenger->GetTypeId() == TYPEID_PLAYER)
            {
                if(escort)
                {
                    escort->AI()->DoAction(0);
                    veh->GetBase()->ToCreature()->SetSpeed(MOVE_FLIGHT, 0.5f);
                }
            }
        }
    }
};

class npc_waypoint_air_balloon : public CreatureScript
{
public:
    npc_waypoint_air_balloon(): CreatureScript("npc_waypoint_air_balloon"){}

    struct npc_waypoint_air_balloonAI : public npc_escortAI
    {
        npc_waypoint_air_balloonAI(Creature* creature) : npc_escortAI(creature){}

        bool VerifPlayer;

        void Reset()
        {
            VerifPlayer = false;
        }

        void DoAction(int32 const action)
        {
            Start(false, true);
        }

        void WaypointReached(uint32 waypointId)
        {
            Player* player = GetPlayerForEscort();

            Creature* aysa = me->FindNearestCreature(56661, 50.00f, true);
            Creature* jipatte = me->FindNearestCreature(56660, 50.00f, true);
            Creature* shen = me->FindNearestCreature(56676, 50.00f, true);
            Creature* mongol = me->FindNearestCreature(55649, 50.00f, true);
            Map* map = me->GetMap();

            switch (waypointId)
            {
                case 1:
                    me->SetSpeed(MOVE_FLIGHT, 0.5f);
                    if(mongol)
                    {
                        mongol->GetMotionMaster()->MovePath(55649, false);
                        mongol->SetSpeed(MOVE_FLIGHT, 0.5f);
                    }
                    if(jipatte)
                        jipatte->AI()->Talk(SAY_JI_BALLON_1);
                    SetEscortPaused(true);
                    VerifPlayer = true;
                    break;
                case 2:
                    if(aysa)
                        aysa->AI()->Talk(SAY_AYSA_BALLON_1);
                    break;
                case 3:
                    if(jipatte)
                        jipatte->AI()->Talk(SAY_JI_BALLON_2);
                    break;
                case 4:
                    if(aysa)
                        aysa->AI()->Talk(SAY_AYSA_BALLON_2);
                    break;
                case 5:
                    if(jipatte)
                        jipatte->AI()->Talk(SAY_JI_BALLON_3);
                    break;
                case 6:
                    if(aysa)
                        aysa->AI()->Talk(SAY_AYSA_BALLON_3);
                    break;
                case 7:
                    if(aysa)
                        aysa->AI()->Talk(SAY_AYSA_BALLON_4);
                    break;
                case 8:
                    me->SetSpeed(MOVE_FLIGHT, 1.0f);
                    if(mongol)
                        mongol->SetSpeed(MOVE_FLIGHT, 1.0f);
                    if(aysa)
                        aysa->AI()->Talk(SAY_AYSA_BALLON_5);
                    SetEscortPaused(true);
                    VerifPlayer = true;
                    break;
                case 9:
                    if(shen)
                        shen->AI()->Talk(SAY_SHEN_ZI_BALLON_1);
                    break;
                case 10:
                    if(shen)
                        shen->AI()->Talk(SAY_SHEN_ZI_BALLON_2);
                    break;
                case 11:
                    if(shen)
                        shen->AI()->Talk(SAY_SHEN_ZI_BALLON_3);
                    break;
                case 12:
                    if(shen)
                        shen->AI()->Talk(SAY_SHEN_ZI_BALLON_4);
                    break;
                case 13:
                    if(aysa)
                        aysa->AI()->Talk(SAY_AYSA_BALLON_6);
                    break;
                case 14:
                    if(shen)
                        shen->AI()->Talk(SAY_SHEN_ZI_BALLON_5);
                    break;
                case 15:
                    if(aysa)
                        aysa->AI()->Talk(SAY_AYSA_BALLON_7);
                    break;
                case 16:
                    if(map)
                    {
                        Map::PlayerList const& players = map->GetPlayers();
                        for(Map::PlayerList::const_iterator iter = players.begin() ; iter != players.end() ; ++iter)
                        {
                            Player *player = iter->getSource();
                            if(player)
                                if (player->isAlive() && player->IsInDist2d(me, 50))
                                    player->KilledMonsterCredit(55939);
                        }
                    }
                    if(shen)
                        shen->AI()->Talk(SAY_SHEN_ZI_BALLON_6);
                    break;
                case 17:
                    if(jipatte)
                        jipatte->AI()->Talk(SAY_JI_BALLON_4);
                    SetEscortPaused(true);
                    VerifPlayer = true;
                    break;
                case 18:
                    me->SetSpeed(MOVE_FLIGHT, 5.0f);
                    if(mongol)
                        mongol->SetSpeed(MOVE_FLIGHT, 5.0f);
                    if(jipatte)
                        jipatte->AI()->Talk(SAY_JI_BALLON_5);
                    break;
                case 19:
                    if(aysa)
                        aysa->AI()->Talk(SAY_AYSA_BALLON_8);
                    break;
                case 21:
                    if(jipatte)
                        jipatte->AI()->Talk(SAY_JI_BALLON_6);
                    SetEscortPaused(true);
                    VerifPlayer = true;
                    break;
                case 23:
                    me->SetSpeed(MOVE_FLIGHT, 3.0f);
                    if(mongol)
                        mongol->SetSpeed(MOVE_FLIGHT, 3.0f);
                    if(aysa)
                        aysa->AI()->Talk(SAY_AYSA_BALLON_9);
                    break;
                case 25:
                    if(jipatte)
                        jipatte->AI()->Talk(SAY_JI_BALLON_7);
                    break;
                case 26:
                    if(aysa)
                        aysa->AI()->Talk(SAY_AYSA_BALLON_10);
                    break;
                case 27:
                    if(aysa)
                        aysa->AI()->Talk(SAY_AYSA_BALLON_11);
                    break;
                case 29:
                    me->DespawnOrUnsummon();
                    break;
            }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            npc_escortAI::UpdateAI(uiDiff);

            if (VerifPlayer)
            {
                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                    if(summoner->ToPlayer())
                        if(summoner->IsInDist2d(me, 10.00f))
                        {
                            SetEscortPaused(false);
                            VerifPlayer = false;
                        }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_waypoint_air_balloonAI(creature);
    }
};

class npc_shang_xi_air_balloon_click : public CreatureScript
{
public:
    npc_shang_xi_air_balloon_click(): CreatureScript("npc_shang_xi_air_balloon_click") { }

    struct npc_shang_xi_air_balloon_clickAI : public ScriptedAI
    {
        npc_shang_xi_air_balloon_clickAI(Creature* creature) : ScriptedAI(creature){}

        void Reset()
        {
            me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
        }

        void OnSpellClick(Unit* clicker)
        {
            Creature* mongol = clicker->FindNearestCreature(55649, 50.00f, true);
            clicker->SummonCreature(31002, 908.82f, 4558.87f, 232.31f, 0.62f, TEMPSUMMON_TIMED_DESPAWN, 600000);

            if(clicker->ToPlayer())
                clicker->ToPlayer()->KilledMonsterCredit(56378);

            if(clicker->GetTypeId() == TYPEID_PLAYER)
                if(mongol)
                    clicker->EnterVehicle(mongol);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shang_xi_air_balloon_clickAI(creature);
    }
};

/* Event Ouverture Porte */

enum Area7710
{
    QUEST_BIDDEN_GREATNESS  = 29792
};

class at_bidden_greatness_door : public AreaTriggerScript
{
public :
    at_bidden_greatness_door() : AreaTriggerScript("at_bidden_greatness_door") {}

    bool OnTrigger(Player* player, const AreaTriggerEntry *at)
    {
        if(player->GetQuestStatus(QUEST_BIDDEN_GREATNESS) == QUEST_STATUS_INCOMPLETE)
        {
            if(player->HasAura(115426))
            {
                player->CastSpell(player, 115337, true); //jojo
                player->CastSpell(player, 115335, true); // ji patte
                player->CastSpell(player, 115332, true); // aysa
                player->RemoveAurasDueToSpell(94568);
                player->RemoveAurasDueToSpell(115426);
                return true;
            }
        }
        return false;
    }
};

enum eAysaEventDoor
{
    SAY_AYSA_DOOR_0     = 0,
    SAY_AYSA_DOOR_1     = 1,
    SAY_AYSA_DOOR_2     = 2,
    SAY_AYSA_DOOR_3     = 3
};

enum eJiEventDoor
{
    SAY_JI_DOOR_1       = 0,
    SAY_JI_DOOR_2       = 1,
    SAY_JI_DOOR_3       = 2,
    SAY_JI_DOOR_4       = 3,
    SAY_JI_DOOR_5       = 4
};

enum eWeiEventDoor
{
    SAY_WEI_DOOR_1      = 0
};

enum eKorgaEventDoor
{
    SAY_KORGA_DOOR_1    = 0,
    SAY_KORGA_DOOR_2    = 1,
    SAY_KORGA_DOOR_3    = 2
};


class npc_aysa_door : public CreatureScript
{
public:
    npc_aysa_door(): CreatureScript("npc_aysa_door") { }

    struct npc_aysa_doorAI : public npc_escortAI
    {
        npc_aysa_doorAI(Creature* creature) : npc_escortAI(creature) {}

        bool VerifPlayer;

        void Reset()
        {
            VerifPlayer = false;
            Talk(SAY_AYSA_DOOR_0);
        }

        void WaypointReached(uint32 waypointId)
        {
            Player* player = GetPlayerForEscort();

            GameObject* mandori = me->FindNearestGameObject(210967, 500.00f);

            switch (waypointId)
            {
                case 1:
                    if(mandori)
                        mandori->SetGoState(GO_STATE_ACTIVE);
                    me->CastSpell(me, 115442, true);
                    break;

                case 12:
                    SetEscortPaused(true);
                    VerifPlayer = true;
                    break;

                case 13:
                    Talk(SAY_AYSA_DOOR_1);
                    me->HandleEmoteCommand(EMOTE_STATE_USE_STANDING);
                    break;

                case 14:
                    Talk(SAY_AYSA_DOOR_2);
                    me->HandleEmoteCommand(EMOTE_STATE_NONE);
                    break;

                case 25:
                    SetEscortPaused(true);
                    VerifPlayer = true;
                    break;

                case 27:
                    Talk(SAY_AYSA_DOOR_3);
                    break;

                case 29:
                    me->GetMotionMaster()->MoveJump(425.79f, 3674.37f, 78.4f, 10, 10);
                    break;

                case 33:
                    me->DespawnOrUnsummon();
                    break;
            }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            npc_escortAI::UpdateAI(uiDiff);

            Start(false, true);

            if (VerifPlayer)
            {
                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                    if(summoner->ToPlayer())
                        if(summoner->IsInDist2d(me, 10.00f))
                        {
                            SetEscortPaused(false);
                            VerifPlayer = false;
                        }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_aysa_doorAI(creature);
    }
};

class npc_jojo_door : public CreatureScript
{
public:
    npc_jojo_door(): CreatureScript("npc_jojo_door") { }

    struct npc_jojo_doorAI : public npc_escortAI
    {
        npc_jojo_doorAI(Creature* creature) : npc_escortAI(creature) {}

        bool VerifPlayer;
        bool VerifPlayer2;

        void Reset()
        {
            me->CastSpell(me, 115672, true);
            VerifPlayer = false;
            VerifPlayer2 = false;
        }

        void WaypointReached(uint32 waypointId)
        {
            Player* player = GetPlayerForEscort();

            GameObject* peiwu = me->FindNearestGameObject(210966, 500.00f);

            switch (waypointId)
            {
                case 10:
                    SetEscortPaused(true);
                    VerifPlayer = true;
                    break;

                case 12:
                    if(peiwu)
                        peiwu->SetGoState(GO_STATE_ACTIVE);
                    me->CastSpell(me, 115443, true);
                    break;

                case 22:
                    SetEscortPaused(true);
                    VerifPlayer2 = true;
                    break;

                case 25:
                    me->GetMotionMaster()->MoveJump(425.79f, 3674.37f, 78.4f, 10, 10);
                    break;

                case 29:
                    me->DespawnOrUnsummon();
                    break;
            }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            npc_escortAI::UpdateAI(uiDiff);

            Start(false, true);

            if (VerifPlayer)
            {
                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                    if(summoner->ToPlayer())
                        if(summoner->IsInDist2d(me, 10.00f))
                        {
                            SetEscortPaused(false);
                            VerifPlayer = false;
                        }
            }

            if (VerifPlayer2)
            {
                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                    if(summoner->ToPlayer())
                        if(summoner->IsInDist2d(me, 15.00f))
                        {
                            SetEscortPaused(false);
                            VerifPlayer2 = false;
                        }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_jojo_doorAI(creature);
    }
};

class npc_ji_door : public CreatureScript
{
public:
    npc_ji_door(): CreatureScript("npc_ji_door") { }

    struct npc_ji_doorAI : public npc_escortAI
    {
        npc_ji_doorAI(Creature* creature) : npc_escortAI(creature) {}

        bool VerifPlayer;

        void Reset()
        {
            VerifPlayer = false;
        }

        void WaypointReached(uint32 waypointId)
        {
            Player* player = GetPlayerForEscort();

            Creature* korga = me->FindNearestCreature(60042, 30.00f, true);
            Creature* wei = me->FindNearestCreature(55943, 30.00f, true);

            switch (waypointId)
            {
                case 12:
                    SetEscortPaused(true);
                    VerifPlayer = true;
                    break;

                case 14:
                    Talk(SAY_JI_DOOR_1);
                    me->HandleEmoteCommand(EMOTE_STATE_USE_STANDING);
                    break;

                case 15:
                    Talk(SAY_JI_DOOR_2);
                    me->HandleEmoteCommand(EMOTE_STATE_NONE);
                    break;

                case 18:
                    Talk(SAY_JI_DOOR_3);
                    break;

                case 26:
                    SetEscortPaused(true);
                    VerifPlayer = true;
                    break;

                case 27:
                    if(korga)
                        korga->AI()->Talk(SAY_KORGA_DOOR_1);
                    break;

                case 28:
                    if(wei)
                        wei->AI()->Talk(SAY_WEI_DOOR_1);
                    break;

                case 29:
                    if(korga)
                        korga->AI()->Talk(SAY_KORGA_DOOR_2);
                    break;

                case 30:
                    Talk(SAY_JI_DOOR_4);
                    break;

                case 31:
                    if(korga)
                        korga->AI()->Talk(SAY_KORGA_DOOR_3);
                    break;

                case 32:
                    Talk(SAY_JI_DOOR_5);
                    break;

                case 33:
                    me->DespawnOrUnsummon();
                    break;
            }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            npc_escortAI::UpdateAI(uiDiff);

            Start(false, true);

            if (VerifPlayer)
            {
                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                    if(summoner->ToPlayer())
                        if(summoner->IsInDist2d(me, 10.00f))
                        {
                            SetEscortPaused(false);
                            VerifPlayer = false;
                        }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ji_doorAI(creature);
    }
};

class npc_ji_lugubre : public CreatureScript
{
public:
    npc_ji_lugubre(): CreatureScript("npc_ji_lugubre") { }

    struct npc_ji_lugubreAI : public npc_escortAI
    {
        npc_ji_lugubreAI(Creature* creature) : npc_escortAI(creature) {}

        void Reset()
        {
        }

        void WaypointReached(uint32 waypointId)
        {
            Player* player = GetPlayerForEscort();

            switch (waypointId)
            {
                case 2:
                    Talk(0);
                    break;

                case 4:
                    me->GetMotionMaster()->MoveJump(424.67f, 3674.40f, 78.4f, 10, 10);
                    break;

                case 35:
                    me->DespawnOrUnsummon();
                    break;
            }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            npc_escortAI::UpdateAI(uiDiff);

            Start(false, true);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ji_lugubreAI(creature);
    }
};


/* Ramassage Marin */

class npc_injured_sailor_click : public CreatureScript
{
public:
    npc_injured_sailor_click(): CreatureScript("npc_injured_sailor_click") { }

    struct npc_injured_sailor_clickAI : public ScriptedAI
    {
        npc_injured_sailor_clickAI(Creature* creature) : ScriptedAI(creature){}

        bool Despawn;

        uint32 Despawn_Timer;

        void Reset()
        {
            me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
            Despawn = false;
        }

        void DoAction(int32 const action)
        {
            Despawn = true;
            Despawn_Timer = 5000;
        }

        void OnSpellClick(Unit* clicker)
        {
            if(clicker->ToPlayer())
            {
                me->EnterVehicle(clicker);
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
            }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if(Despawn)
            {
                if(Despawn_Timer <= uiDiff)
                {
                    me->DisappearAndDie();
                }
                else
                    Despawn_Timer -= uiDiff;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_injured_sailor_clickAI(creature);
    }
};

enum Area7087
{
    QUEST_NONE_LEFT_BEHIND  = 29794
};

class at_none_left_behind : public AreaTriggerScript
{
public :
    at_none_left_behind() : AreaTriggerScript("at_none_left_behind") {}

    bool OnTrigger(Player* player, const AreaTriggerEntry *at)
    {
        if(player->GetQuestStatus(QUEST_NONE_LEFT_BEHIND) == QUEST_STATUS_INCOMPLETE)
        {
            if(player->IsVehicle() && player->HasAura(129340))
            {
                Unit* sailor = player->GetVehicleKit()->GetPassenger(0);

                if(sailor && sailor->ToCreature())
                    sailor->ToCreature()->AI()->Talk(0, player->GetGUID());

                player->RemoveAurasDueToSpell(129340);
                player->KilledMonsterCredit(55999);

                return true;
            }
        }
        return false;
    }
};

class spell_rescue_injured_sailor : public SpellScriptLoader
{
public :
    spell_rescue_injured_sailor() : SpellScriptLoader("spell_rescue_injured_sailor") {}

    class spell_rescue_injured_sailor_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_rescue_injured_sailor_AuraScript);


        void HandleEffectRemove(AuraEffect const* auraEff, AuraEffectHandleModes mode)
        {
            if(GetCaster())
                if(Vehicle* vehicle = GetCaster()->GetVehicleKit())
                    if(Unit* unit = vehicle->GetPassenger(0))
                        if(unit && unit->ToCreature())
                            unit->ToCreature()->AI()->DoAction(0);
        }

        void Register()
        {
            OnEffectRemove += AuraEffectRemoveFn(spell_rescue_injured_sailor_AuraScript::HandleEffectRemove, EFFECT_0, SPELL_AURA_SET_VEHICLE_ID, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_rescue_injured_sailor_AuraScript();
    }
};

class npc_vordraka : public CreatureScript
{
public:
    npc_vordraka(): CreatureScript("npc_vordraka") { }

    struct npc_vordrakaAI : public ScriptedAI
    {
        npc_vordrakaAI(Creature* creature) : ScriptedAI(creature), Summons(me){}

        uint32 Deep_Sea_Smash_Timer;
        uint32 Deep_Sea_Rupture_Timer;

        bool Summon_Timer_1;
        bool Summon_Timer_2;
        bool Say20;
        bool SaySmash;

        SummonList Summons;

        void Reset()
        {
            Deep_Sea_Smash_Timer = 10000;
            Deep_Sea_Rupture_Timer = 20000;

            Summon_Timer_1 = true;
            Summon_Timer_2 = true;
            Say20 = true;
            SaySmash = true;

            Summons.DespawnAll();
        }

        void EnterCombat(Unit* /*who*/)
        {
            me->SummonCreature(56417, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 300000);

            Creature* aysa = me->FindNearestCreature(56417, 100.00f, true);
            if(aysa)
                aysa->AI()->Talk(0);
        }

        void JustSummoned(Creature* Summoned)
        {
            Summons.Summon(Summoned);

            if(Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 500.0f, true))
                if(target && target->GetTypeId() == TYPEID_PLAYER)
                    if(target->InSamePhase(2) && target->isInCombat())
                    {
                        Summoned->CastSpell(target, 117407, false);
                        Summoned->AI()->AttackStart(target);
                    }
        }

        void JustDied(Unit *pWho)
        {
            Creature* aysa = me->FindNearestCreature(56417, 100.00f, true);
            Summons.DespawnAll();

            if(aysa)
            {
                aysa->AI()->Talk(4);
                aysa->GetMotionMaster()->MovePoint(1, 286.39f, 4004.87f, 75.00f);
            }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if(!UpdateVictim())
                return;

            if(me->HealthBelowPct(66) && Summon_Timer_1)
            {
                Creature* aysa = me->FindNearestCreature(56417, 100.00f, true);
                if(aysa)
                    aysa->AI()->Talk(2);

                me->SummonCreature(60685, 256.02f, 3963.01f, 75.00f, 0, TEMPSUMMON_TIMED_DESPAWN, 300000);
                me->SummonCreature(60685, 249.40f, 3972.39f, 76.00f, 0, TEMPSUMMON_TIMED_DESPAWN, 300000);
                Summon_Timer_1 = false;
            }

            if(me->HealthBelowPct(33) && Summon_Timer_2)
            {
                me->SummonCreature(60685, 266.57f, 4014.58f, 80.00f, 0, TEMPSUMMON_TIMED_DESPAWN, 300000);
                me->SummonCreature(60685, 271.92f, 4018.93f, 81.00f, 0, TEMPSUMMON_TIMED_DESPAWN, 300000);
                Summon_Timer_2 = false;
            }

            if(me->HealthBelowPct(20) && Say20)
            {
                Creature* aysa = me->FindNearestCreature(56417, 100.00f, true);
                if(aysa)
                    aysa->AI()->Talk(3);

                Say20 = false;
            }

            if(Deep_Sea_Smash_Timer <= uiDiff)
            {
                if(SaySmash)
                {
                    Creature* aysa = me->FindNearestCreature(56417, 100.00f, true);
                    if(aysa)
                        aysa->AI()->Talk(1);

                    SaySmash = false;
                }

                Creature* aysa = me->FindNearestCreature(56417, 100.00f, true);
                if(aysa)
                    aysa->AI()->DoAction(0);

                DoCast(117287);

                Deep_Sea_Smash_Timer = 20000;
            }
            else
                Deep_Sea_Smash_Timer -= uiDiff;

            if(Deep_Sea_Rupture_Timer <= uiDiff)
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_FARTHEST))
                    me->CastSpell(target, 117456, false);

                Deep_Sea_Rupture_Timer = 20000;
            }
            else
                Deep_Sea_Rupture_Timer -= uiDiff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_vordrakaAI(creature);
    }
};

class npc_aysa_attack_vordraka : public CreatureScript
{
public:
    npc_aysa_attack_vordraka(): CreatureScript("npc_aysa_attack_vordraka") { }

    struct npc_aysa_attack_vordrakaAI : public ScriptedAI
    {
        npc_aysa_attack_vordrakaAI(Creature* creature) : ScriptedAI(creature){}

        uint32 Taunt_Timer;

        void Reset()
        {
            Taunt_Timer = 1000;
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type == POINT_MOTION_TYPE && id == 1)
            {
                me->DespawnOrUnsummon();
            }
        }

        void DoAction(int32 const action)
        {
            Creature* vordraka = me->FindNearestCreature(56009, 200.00f, true);
            if(vordraka)
                JumpBehindTarget(vordraka);
        }

        void JumpBehindTarget(WorldObject const* target)
        {
            Position const targetExactPosition = {target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), target->GetOrientation()};

            float orientation = targetExactPosition.m_orientation + static_cast<float>(M_PI);

            orientation = Position::NormalizeOrientation(orientation);

            float x = targetExactPosition.m_positionX + 0.5f * cos(orientation);
            float y = targetExactPosition.m_positionY + 0.5f * sin(orientation);
            float z = targetExactPosition.m_positionZ;

            Position const exactJumpPosition = {x, y, z, targetExactPosition.m_orientation};

            float speedZ = 10.0f ;
            float speedXY = me->GetExactDist2d(target);

            me->GetMotionMaster()->MoveJump(exactJumpPosition, speedXY, speedZ);
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if(me->HealthBelowPct(50))
            {
                me->SetHealth(me->GetMaxHealth());
            }

            if(Taunt_Timer <= uiDiff)
            {
                Creature* vordraka = me->FindNearestCreature(56009, 200.00f, true);

                if(vordraka)
                {
                    me->CastSpell(vordraka, 114915, true);
                    me->AI()->AttackStart(vordraka);
                }

                Taunt_Timer = 5000;
            }
            else
                Taunt_Timer -= uiDiff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_aysa_attack_vordrakaAI(creature);
    }
};

class npc_aysa_boat : public CreatureScript
{
public:
    npc_aysa_boat(): CreatureScript("npc_aysa_boat") { }

    struct npc_aysa_boatAI : public npc_escortAI
    {
        npc_aysa_boatAI(Creature* creature) : npc_escortAI(creature) {}

        void Reset()
        {
        }

        void WaypointReached(uint32 waypointId)
        {
            Player* player = GetPlayerForEscort();

            switch (waypointId)
            {
                case 1:
                    if(Unit* summoner = me->ToTempSummon()->GetSummoner())
                        Talk(0, summoner->GetGUID());
                    break;

                case 18:
                    Talk(1);
                    break;

                case 19:
                    Talk(2);
                    break;

                case 20:
                    Talk(3);
                    break;

                case 21:
                    Talk(4);
                    break;

                case 22:
                    Talk(5);
                    break;

                case 24:
                    me->GetMotionMaster()->MoveJump(228.98f, 3938.96f, 62.5f, 10, 10);

                    if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                        if(summoner->ToPlayer())
                        {
                            summoner->CastSpell(summoner, 117615, true);
                            summoner->ToPlayer()->SendMovieStart(117);
                        }
                    break;

                case 25:
                    me->DespawnOrUnsummon();
                    break;
            }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            npc_escortAI::UpdateAI(uiDiff);

            Start(false, true);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_aysa_boatAI(creature);
    }
};

class npc_ji_boat : public CreatureScript
{
public:
    npc_ji_boat(): CreatureScript("npc_ji_boat") { }

    struct npc_ji_boatAI : public npc_escortAI
    {
        npc_ji_boatAI(Creature* creature) : npc_escortAI(creature) {}

        void Reset()
        {
        }

        void WaypointReached(uint32 waypointId)
        {
            Player* player = GetPlayerForEscort();

            switch (waypointId)
            {
                case 2:
                    Talk(0);
                    break;

                case 3:
                    Talk(1);
                    break;

                case 4:
                    Talk(2);
                    break;

                case 5:
                    me->HandleEmoteCommand(EMOTE_STATE_USE_STANDING);
                    break;

                case 6:
                    me->HandleEmoteCommand(EMOTE_STATE_NONE);
                    break;

                case 8:
                    me->GetMotionMaster()->MoveJump(228.98f, 3938.96f, 62.5f, 10, 10);
                    break;

                case 9:
                    me->DespawnOrUnsummon();
                    break;
            }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            npc_escortAI::UpdateAI(uiDiff);

            Start(false, true);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ji_boatAI(creature);
    }
};

/**************************/
/****** Final Event *******/
/**************************/

class npc_trigger_healing_shen: public CreatureScript
{
public:
    npc_trigger_healing_shen(): CreatureScript("npc_trigger_healing_shen") { }

    struct npc_trigger_healing_shenAI : public ScriptedAI
    {
        npc_trigger_healing_shenAI(Creature* creature) : ScriptedAI(creature){}

        uint32 PainShake_Timer;
        uint32 Healing_Timer;

        void Reset()
        {
            PainShake_Timer = 1000;
            Healing_Timer = 1500;
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if(PainShake_Timer <= uiDiff)
            {
                PainShake();
                PainShake_Timer = 30000;
            }
            else
                PainShake_Timer -= uiDiff;

            if(Healing_Timer <= uiDiff)
            {
                VerifPlayer();
                Healing_Timer = 1000;
            }
            else
                Healing_Timer -= uiDiff;
        }

        void VerifPlayer()
        {
            Map* map = me->GetMap();
            Map::PlayerList const& pl = map->GetPlayers();

            for(Map::PlayerList::const_iterator iter = pl.begin() ; iter != pl.end() ; ++iter)
            {
                if(Player* player = iter->getSource())
                {
                    if(!player->HasAura(117783) && player->GetQuestStatus(29799) == QUEST_STATUS_INCOMPLETE)
                    {
                        player->CastSpell(player, 117783, true);
                        player->SetMaxPower(POWER_ALTERNATE_POWER, 700);
                        player->SetPower(POWER_ALTERNATE_POWER, 0);
                    }

                    if(player->HasAura(117783) && player->GetQuestStatus(29799) == QUEST_STATUS_NONE)
                    {
                        player->RemoveAurasDueToSpell(117783);
                    }
                }
            }
        }

        void PainShake()
        {
            Map* map = me->GetMap();
            Map::PlayerList const& pl = map->GetPlayers();

            for(Map::PlayerList::const_iterator iter = pl.begin() ; iter != pl.end() ; ++iter)
                if(Player* player = iter->getSource())
                    if(player->isAlive() && player->InSamePhase(1024))
                        if(player->GetAreaId() == 5833)
                            player->CastSpell(player, 117969, true);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_trigger_healing_shenAI(creature);
    }
};

class npc_ji_healing_shen: public CreatureScript
{
public:
    npc_ji_healing_shen(): CreatureScript("npc_ji_healing_shen") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == 29799)
        {
            player->SendUpdateWorldState(6489, 1);
            player->SendUpdateWorldState(6488, 1);
        }
        return true;
    }

    struct npc_ji_healing_shenAI : public ScriptedAI
    {
        npc_ji_healing_shenAI(Creature* creature) : ScriptedAI(creature){}

        uint32 Text_Timer;

        void Reset()
        {
            Text_Timer = 1000;
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if(Text_Timer <= uiDiff)
            {
                Talk(0);
                Text_Timer = 120000;
            }
            else
                Text_Timer -= uiDiff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ji_healing_shenAI(creature);
    }
};

class npc_healer_shen_wreckage: public CreatureScript
{
public:
    npc_healer_shen_wreckage(): CreatureScript("npc_healer_shen_wreckage") { }

    struct npc_healer_shen_wreckageAI : public ScriptedAI
    {
        npc_healer_shen_wreckageAI(Creature* creature) : ScriptedAI(creature){}

        uint32 Healing_timer;

        bool HealingShen;

        void Reset()
        {
            me->SetHealth(me->GetMaxHealth() / 4);
            me->SetStandState(UNIT_STAND_STATE_SLEEP);
            me->SetReactState(REACT_PASSIVE);
            me->setFaction(35);
            me->CastSpell(me, 117857, true);
            HealingShen = false;
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type == POINT_MOTION_TYPE && id == 1)
            {
                me->CastSpell(me, 117932, true);
                me->setFaction(42);
                Healing_timer = 2000;
                HealingShen = true;
            }
        }

        void DoAction(int32 const action)
        {
            me->SetStandState(UNIT_STAND_STATE_STAND);
            me->CastSpell(me, 117934, false);
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == 117934)
            {
                Talk(0);
                me->SetHealth(me->GetMaxHealth());
                me->GetMotionMaster()->MovePoint(1, 254.25f, 3959.83f, 65.0f, true);
            }
        }

        void AddPowerToPlayersOnMap()
        {
            Map* map = me->GetMap();
            if(map)
            {
                Map::PlayerList const& players = map->GetPlayers();

                if(players.isEmpty())
                    return;

                for(Map::PlayerList::const_iterator iter = players.begin() ; iter != players.end() ; ++iter)
                {
                    Player* player = iter->getSource();
                    if(player)
                        if (player->isAlive() && player->GetQuestStatus(29799) == QUEST_STATUS_INCOMPLETE)
                            if(player->GetAreaId() == 5833)
                                if(player->HasAura(117783))
                                    AddPower(player);
                }
            }
        }

        void AddPower(Player* player)
        {
            if(player)
            {
                player->ModifyPower(POWER_ALTERNATE_POWER, +2);

                switch(player->GetPower(POWER_ALTERNATE_POWER))
                {
                    case 700 :
                        player->KilledMonsterCredit(56011);
                        player->RemoveAura(117783);
                        break ;

                    default :
                        break ;
                }
            }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if(HealingShen)
            {
                if(Healing_timer <= uiDiff)
                {
                    AddPowerToPlayersOnMap();
                    Healing_timer = 2000;
                }
                else
                    Healing_timer -= uiDiff;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_healer_shen_wreckageAI(creature);
    }
};

class npc_healer_shen: public CreatureScript
{
public:
    npc_healer_shen(): CreatureScript("npc_healer_shen") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if(player->hasQuest(29799))
            if(!creature->isInCombat())
            {
                creature->AI()->DoAction(0);
                creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }

        return true;
    }

    struct npc_healer_shenAI : public ScriptedAI
    {
        npc_healer_shenAI(Creature* creature) : ScriptedAI(creature), Summons(me){}

        uint32 VerifCombat_Timer;
        uint32 Cast_Timer;
        uint32 Healing_timer;
        uint32 Pop_timer;
        SummonList Summons;

        bool HealingShen;

        void Reset()
        {
            VerifCombat_Timer = 1000;
            Cast_Timer = 1000;
            me->setFaction(42);
            HealingShen = false;
        }

        void JustSummoned(Creature* Summoned)
        {
            Summons.Summon(Summoned);

            Summoned->GetMotionMaster()->MoveJump(253.51f, 3954.70f, 66.00f, 20, 20);
            Summoned->AI()->AttackStart(me);
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type == POINT_MOTION_TYPE && id == 1)
            {
                me->CastSpell(me, 117932, true);
                me->SetReactState(REACT_PASSIVE);
                me->setFaction(42);
                Healing_timer = 2000;
                Pop_timer = 4000;
                HealingShen = true;
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }
        }

        void DoAction(int32 const action)
        {
            switch (action)
            {
                case 0:
                    Talk(0);
                    me->GetMotionMaster()->MovePoint(1, 254.25f, 3959.83f, 65.0f, true);
                    me->SetReactState(REACT_PASSIVE);
                    me->setFaction(35);
                    break;
            }
        }

        void AddPowerToPlayersOnMap()
        {
            Map* map = me->GetMap();
            if(map)
            {
                Map::PlayerList const& players = map->GetPlayers();

                if(players.isEmpty())
                    return;

                for(Map::PlayerList::const_iterator iter = players.begin() ; iter != players.end() ; ++iter)
                {
                    Player* player = iter->getSource();
                    if(player)
                        if (player->isAlive() && player->GetQuestStatus(29799) == QUEST_STATUS_INCOMPLETE)
                            if(player->GetAreaId() == 5833)
                                if(player->HasAura(117783))
                                    AddPower(player);
                }
            }
        }

        void AddPower(Player* player)
        {
            if(player)
            {
                player->ModifyPower(POWER_ALTERNATE_POWER, +2);

                switch(player->GetPower(POWER_ALTERNATE_POWER))
                {
                    case 700:
                        player->KilledMonsterCredit(56011);
                        player->RemoveAura(117783);
                        break ;

                    default :
                        break ;
                }
            }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if(HealingShen)
            {
                if(Healing_timer <= uiDiff)
                {
                    AddPowerToPlayersOnMap();
                    Healing_timer = 2000;
                }
                else
                    Healing_timer -= uiDiff;

                if(Pop_timer <= uiDiff)
                {
                    me->SummonCreature(60780, 215.76f, 3950.22f, 72.00f, 0, TEMPSUMMON_TIMED_DESPAWN, 60000);
                    me->SummonCreature(60858, 288.58f, 3939.21f, 87.00f, 0, TEMPSUMMON_TIMED_DESPAWN, 60000);
                    Pop_timer = 25000;
                }
                else
                    Pop_timer -= uiDiff;
            }

            if(!HealingShen)
            {
                if(me->HealthBelowPct(30))
                {
                    me->SetHealth(me->GetMaxHealth());
                }

                if(VerifCombat_Timer <= uiDiff)
                {
                    if(me->isInCombat())
                        me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

                    if(!me->isInCombat())
                        me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

                    me->SetReactState(REACT_DEFENSIVE);
                    VerifCombat_Timer = 1000;
                }
                else
                    VerifCombat_Timer -= uiDiff;

                if(!UpdateVictim())
                    return;

                if(Cast_Timer <= uiDiff)
                {
                    if(me->GetEntry() == 60877)
                        me->CastSpell(me->getVictim(), 117935, false);

                    if(me->GetEntry() == 60770)
                        me->CastSpell(me->getVictim(), 117767, false);

                    Cast_Timer = 3000;
                }
                else
                    Cast_Timer -= uiDiff;

                DoMeleeAttackIfReady();
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_healer_shenAI(creature);
    }
};

class npc_wreckage: public CreatureScript
{
public:
    npc_wreckage(): CreatureScript("npc_wreckage") { }

    struct npc_wreckageAI : public ScriptedAI
    {
        npc_wreckageAI(Creature* creature) : ScriptedAI(creature){}

        void Reset()
        {
            me->CastSpell(me, 117855, true);
        }

        void MovementInform(uint32 type, uint32 id)
        {
            switch (id)
            {
                case EVENT_JUMP:
                    me->DespawnOrUnsummon();
                    break;
            }
        }

        void OnSpellClick(Unit* clicker)
        {
            me->GetMotionMaster()->MoveJump(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 10, 10, 10, EVENT_JUMP);

            Creature* healerA = me->FindNearestCreature(60878, 10.0f, true);
            Creature* healerH = me->FindNearestCreature(60834, 10.0f, true);

            if(healerA)
                healerA->AI()->DoAction(0);

            if(healerH)
                healerH->AI()->DoAction(0);
        }

        void UpdateAI(const uint32 uiDiff)
        {
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wreckageAI(creature);
    }
};

class npc_deepscale_fleshripper: public CreatureScript
{
public:
    npc_deepscale_fleshripper(): CreatureScript("npc_deepscale_fleshripper") { }

    struct npc_deepscale_fleshripperAI : public ScriptedAI
    {
        npc_deepscale_fleshripperAI(Creature* creature) : ScriptedAI(creature){}

        uint32 RipFlesh_Timer;

        void Reset()
        {
            RipFlesh_Timer = 5000;
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if(!UpdateVictim())
                return;

            if(RipFlesh_Timer <= uiDiff)
            {
                me->CastSpell(me, 128533, false);
                RipFlesh_Timer = 10000;
            }
            else
                RipFlesh_Timer -= uiDiff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_deepscale_fleshripperAI(creature);
    }
};

/**************************/
/**** END Final Event *****/
/**************************/


class npc_korga_strongmane: public CreatureScript
{
public:
    npc_korga_strongmane(): CreatureScript("npc_korga_strongmane") { }

    struct npc_korga_strongmaneAI : public ScriptedAI
    {
        npc_korga_strongmaneAI(Creature* creature) : ScriptedAI(creature){}

        uint32 Talk_1_Timer;
        uint32 Talk_2_Timer;

        void Reset()
        {
            Talk_1_Timer = 1000;
            Talk_2_Timer = 11000;
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if (Talk_1_Timer <= uiDiff)
            {
                Talk(0);
                Talk_1_Timer = 60000;
            }
            else Talk_1_Timer -= uiDiff;

            if (Talk_2_Timer <= uiDiff)
            {
                Creature* delora = me->FindNearestCreature(60889, 20.00f, true);
                if(delora)
                    delora->AI()->Talk(0);

                Talk_2_Timer = 60000;
            }
            else Talk_2_Timer -= uiDiff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_korga_strongmaneAI(creature);
    }
};

/*######
## npc_nourished_yak_escort_3
######*/

class npc_nourished_yak_escort_3 : public CreatureScript
{
public:
    npc_nourished_yak_escort_3(): CreatureScript("npc_nourished_yak_escort_3") { }

    struct npc_nourished_yak_escort_3AI : public npc_escortAI
    {
        npc_nourished_yak_escort_3AI(Creature* creature) : npc_escortAI(creature) {}

        void Reset()
        {
            me->CastSpell(me, 111810, true);
            me->SetPhaseMask(2049, true);
            me->setFaction(35);
        }

        void WaypointReached(uint32 waypointId)
        {
            Player* player = GetPlayerForEscort();

            switch (waypointId)
            {
                case 1:
                    SetRun();
                    break;
                case 32:
                    if (Creature* chariot = me->FindNearestCreature(57740, 50.00f, true))
                        chariot->DespawnOrUnsummon();

                    me->DespawnOrUnsummon();
                    break;

            }
        }

        void OnCharmed(bool /*apply*/){}

        void UpdateAI(const uint32 uiDiff)
        {
            npc_escortAI::UpdateAI(uiDiff);

            Start(false, true);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_nourished_yak_escort_3AI(creature);
    }
};

/*######
## npc_delivery_cart_escort_3
######*/

class npc_delivery_cart_escort_3 : public CreatureScript
{
public:
    npc_delivery_cart_escort_3(): CreatureScript("npc_delivery_cart_escort_3") { }

    struct npc_delivery_cart_escort_3AI : public npc_escortAI
    {
        npc_delivery_cart_escort_3AI(Creature* creature) : npc_escortAI(creature) {}

        uint32 StartTimer;

        void Reset()
        {
            me->CastSpell(me, 108692, true);
            me->CastSpell(me, 111809, true);
            me->SetPhaseMask(2049, true);
            me->setFaction(35);
            StartTimer = 800;
        }

        void WaypointReached(uint32 waypointId)
        {
            Player* player = GetPlayerForEscort();

            switch (waypointId)
            {
                case 1:
                    SetRun();
                    break;
                case 32:
                    me->DespawnOrUnsummon();
                    break;

            }
        }

        void OnCharmed(bool /*apply*/){}

        void UpdateAI(const uint32 uiDiff)
        {
            npc_escortAI::UpdateAI(uiDiff);

            if (StartTimer <= uiDiff)
            {
                Start(false, true);
                StartTimer  = 300000;
            }
            else StartTimer -= uiDiff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_delivery_cart_escort_3AI(creature);
    }
};

class npc_master_shang_xi_spirit : public CreatureScript
{
public:
    npc_master_shang_xi_spirit() : CreatureScript("npc_master_shang_xi_spirit") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->isQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if(player->hasQuest(31450))
        {
            QueryResult result = WorldDatabase.Query("SELECT text FROM creature_text WHERE entry = 560130");

            if (!result)
                return false;

            std::string text;

            do
            {
                Field* neutralFaction = result->Fetch();
                text = neutralFaction[0].GetString();
            } while (result->NextRow());

            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, text , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        }

        player->PlayerTalkClass->SendGossipMenu(724006, creature->GetGUID());

        return true;
    }

    bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF + 1:
            {
                WorldPacket data(SMSG_SHOW_NEUTRAL_PLAYER_FACTION_SELECT_UI);
                player->GetSession()->SendPacket(&data);
            }
            break;
        }

        player->PlayerTalkClass->SendCloseGossip();
        return true;
    }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == 31450)
        {
            creature->AI()->DoAction(ACTION_SAY);
        }
        return true;
    }

    struct npc_master_shang_xi_spiritAI : public ScriptedAI
    {
            npc_master_shang_xi_spiritAI(Creature* creature) : ScriptedAI(creature) {}

            bool StartEvent;
            uint32 Talk_1_Timer;
            uint32 Talk_2_Timer;
            uint32 Talk_3_Timer;
            uint32 Talk_4_Timer;
            uint32 Talk_5_Timer;
            uint32 Talk_6_Timer;

            void Reset()
            {
                StartEvent = false;
                Talk_1_Timer = 1000;
                Talk_2_Timer = 11000;
                Talk_3_Timer = 21000;
                Talk_4_Timer = 29000;
                Talk_5_Timer = 37000;
                Talk_6_Timer = 47000;
            }

            void DoAction(int32 const action)
            {
                switch (action)
                {
                    case ACTION_SAY:
                        StartEvent = true;
                        break;
                }
            }

            void UpdateAI(const uint32 uiDiff)
            {
                if(StartEvent)
                {
                    if (Talk_1_Timer <= uiDiff)
                    {
                        Talk(0);
                        Talk_1_Timer = 60000;
                    }
                    else Talk_1_Timer -= uiDiff;

                    if (Talk_2_Timer <= uiDiff)
                    {
                        Talk(1);
                        Talk_2_Timer = 60000;
                    }
                    else Talk_2_Timer -= uiDiff;

                    if (Talk_3_Timer <= uiDiff)
                    {
                        Creature* aysa = me->FindNearestCreature(57721, 20.0f, true);
                        if(aysa)
                            aysa->AI()->Talk(0);

                        Talk_3_Timer = 60000;
                    }
                    else Talk_3_Timer -= uiDiff;

                    if (Talk_4_Timer <= uiDiff)
                    {
                        Talk(2);
                        Talk_4_Timer = 60000;
                    }
                    else Talk_4_Timer -= uiDiff;

                    if (Talk_5_Timer <= uiDiff)
                    {
                        Creature* ji = me->FindNearestCreature(57720, 20.0f, true);
                        if(ji)
                            ji->AI()->Talk(0);

                        Talk_5_Timer = 60000;
                    }
                    else Talk_5_Timer -= uiDiff;

                    if (Talk_6_Timer <= uiDiff)
                    {
                        Talk(3);
                        Talk_4_Timer = 60000;
                        Reset();
                        StartEvent = false;
                    }
                    else Talk_6_Timer -= uiDiff;
                }
            }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_master_shang_xi_spiritAI(creature);
    }
};

void AddSC_wandering_isle()
{
    new at_meditation_pool();
    new at_huojin_monk_talk();
    new at_jaomin_ro_talk();
    new at_trainee_nim_talk();
    new at_trainee_guang_talk();
    new at_area_7748_talk();
    new at_ji_firepaw_talk();
    new at_area_7750_talk();
    new at_pop_child_panda();
    new at_delivery_cart_talk();
    new at_delivery_cart_talk_2();
    new at_7106_mongolfiere();
    new npc_first_quest_pandaren();	
    new npc_trainee();
    new npc_zhi();
    new areatrigger_at_the_missing_driver();
    new npc_min_dimwind_pop();
    new npc_aysa_cloudsinger_pop();
    new npc_aysa_cloudsinger_meditation();
    new npc_living_air();
    new spell_blessing_flamme_panda();
    new npc_huo_escort();
    new npc_deng_child();
    new npc_cai_child();
    //new at_test_etang();
    new npc_balance_pole();
    new npc_balance_pole_finish();
    new npc_tushui_monk();
    new npc_jojo_ironbrow();
    new at_the_spirit_of_water();
    new npc_shu_reflexion();
    new npc_water_spout_bunny();
    new npc_shu_escort();
    new npc_nourished_yak_escort();
    new npc_delivery_cart_escort();
    new npc_jojo_ironbrow_plank();
	new npc_shu_quest_29774();
    new npc_shu_escort_wugou();
    new npc_wugou_escort();
    new npc_nourished_yak_escort_2();
    new npc_delivery_cart_escort_2();
    new npc_uplifting_draft();
    new npc_master_shang_xi_temple();
    new npc_ji_firepaw_escort();
    new at_pop_zhao_ren();
    new at_zan_talk();
    new npc_zhao_ren_pop();
    new npc_head_shen_zin_su();
    new npc_jojo_ironbrow_stack();
    new npc_jojo_ironbrow_pillar();
	new spell_shoot_all_the_fireworks_periodic();
    new npc_huojin_monk_escort();
	new spell_ruk_ruk_ooksplosions();
    new npc_ruk_ruk();
	new mob_ruk_ruk_rocket();
    new at_wind_chamber();
    new npc_aysa_wind_escort();
    new npc_zhao_ren();
    new npc_firework_launcher();
    new npc_ji_firepaw_zhao();
    new npc_aysa_wind_exit_escort();
    new npc_dafeng_escort();
    new npc_master_shang_xi_escort();
    new npc_master_shang_xi_dead();
    new npc_aysa_7106();
    new npc_ji_7106();
    new npc_shang_xi_air_balloon();
    new npc_waypoint_air_balloon();
    new npc_shang_xi_air_balloon_click();
    new at_bidden_greatness_door();
    new npc_aysa_door();
    new npc_jojo_door();
    new npc_ji_door();
    new npc_ji_lugubre();
    new npc_injured_sailor_click();
    new at_none_left_behind();
    new spell_rescue_injured_sailor();
    new npc_vordraka();
    new npc_aysa_attack_vordraka();
    new npc_aysa_boat();
    new npc_ji_boat();
    new npc_trigger_healing_shen();
    new npc_ji_healing_shen();
    new npc_healer_shen_wreckage();
    new npc_healer_shen();
    new npc_wreckage();
    new npc_deepscale_fleshripper();
    new npc_korga_strongmane();
    new npc_nourished_yak_escort_3();
    new npc_delivery_cart_escort_3();
    new npc_master_shang_xi_spirit();
}
