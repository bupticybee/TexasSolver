import numpy as np
import os
import sys
import json
from copy import deepcopy
import networkx as nx
import matplotlib.pyplot as plt
from networkx.drawing.nx_agraph import graphviz_layout

class Struct:
    def __init__(self, **entries):
        self.__dict__.update(entries)

class RulesBuilder():
    """
    two card texas holdem game tree builder
    """
    def __init__(self,
                 rule,
                 current_commit = [2,2],
                 current_round =  2,
                 raise_limit = 1,
                 check_limit = 2,
                 small_blind = 0.5,
                 big_blind = 1,
                 stack = 10,
                 bet_sizes = ["1_pot"],
                 raise_sizes = ["1_pot"],
                 allin_threshold = 0.67,
                 ):
        self.rule = rule
        self.parse_rules()

        self.current_commit = current_commit
        self.current_round = current_round
        self.raise_limit = raise_limit
        self.check_limit = check_limit
        self.small_blind = small_blind
        self.big_blind = big_blind
        self.stack = stack
        self.bet_sizes = bet_sizes
        self.raise_sizes = raise_sizes
        self.allin_threshold = allin_threshold
        self.initial_effective_stack = stack - self.current_commit[0]

    def parse_rules(self):
        for k,v in self.rule.items():
            setattr(self,k,v)
        rule = Struct(**self.rule)
        self.rule = rule

    def get_beginning_chip(self):
        assert(self.players >= 2)
        # 中间不下注的人的 bet 都是0
        midbets = []
        for i in range(self.players - 2):
            midbets.append(0)
        chips = [self.small_blind] + midbets + [self.big_blind]
        return chips

def raise_number_this_round(root):
    if root is None:
        return 0
    elif not isinstance(root,ActionNode):
        return 0
    elif isinstance(root,DealCardNode):
        return 0
    elif 'raise' in root.last_action:
        return 1 + raise_number_this_round(root.parent)
    elif 'raise' not in root.last_action:
        return raise_number_this_round(root.parent)
    elif root.last_action == 'begin':
        return 0
    else:
        raise

def check_number_this_round(root):
    if root is None:
        return 0
    elif not isinstance(root,ActionNode):
        return 0
    elif isinstance(root,DealCardNode):
        return 0
    elif 'check' in root.last_action:
        return 1 + check_number_this_round(root.parent)
    elif 'check' not in root.last_action:
        return check_number_this_round(root.parent)
    elif root.last_action == 'begin':
        return 0
    else:
        raise

class TreeBuilder:
    def __init__(self,rule):
        self.rule = rule
        self.build_tree()

    def build_tree(self):
        pass

    def format_tree(self,depth_limit=None):
        self.formatted_arr = []
        formatted_arr = self.formatted_arr

        self.__format_tree(self.root,1,depth_limit)

        return formatted_arr

    def plot_tree(self,jupyter=True,depth_limit = 50,show=False):
        # not jupyter plot not supportted
        assert jupyter

        tree_tovis = self.format_tree(depth_limit=depth_limit)
        G = nx.DiGraph()

        # Build up a graph
        for t in tree_tovis:
            G.add_edge(t[0], t[1])

        # Plot trees
        pos=graphviz_layout(G, prog='dot')
        nx.draw(G, pos, with_labels=True, arrows=False)

        if show:
            plt.show()

    def __format_tree(self,root,depth,limit):
        if limit is not None and depth > limit:
            return
        if root.children is not None:
            for one_child in root.children.values():
                self.formatted_arr.append([root.to_string(),one_child.to_string()])
                self.__format_tree(one_child,depth + 1,limit)

    def gen_km_json(self,json_file,path_prefix=[],limit=np.inf,ret_json=False):
        """
        利用百度脑图的格式可视化决策树
        """
        root = self.root
        for one_action in path_prefix:
            root = root.children[one_action]
        retjson = self.__gen_km_json(root,depth = 0,limit=limit)
        with open(json_file,'w') as whdl:
            json.dump({'root':retjson},whdl)
        if ret_json == True:
            return retjson

    def __gen_km_json(self,root,depth,limit,parent=None):
        if limit is not None and depth > limit:
            return
        children = []
        children_actions = []
        text = root.to_string()
        node_type = "Action"

        if "dealcard" in text and "dealcard" in parent.to_string():
            text = "Chance [DealCard]"

        text += "\nround: {}".format({1:"preflop",2:"flop",3:"turn",4:"river"}[root.betting_round])
        if "dealcard" in text and "dealcard" not in parent.to_string():
            node_type = "Chance"
        if root.terminal == True:
            node_type = "Terminal"
        elif root.showdown == True:
            if root.betting_round == 4:
                node_type = "Showdown"
            else:
                node_type = "Chance"

        one_json = {
            "data": {
                "text": text},
            "children": children,
            "children_actions": children_actions,
            "font-weight": "bold",
            "background": "#73a1bf",
            "resource": [],
            "meta":{
                "round": {1:"preflop",2:"flop",3:"turn",4:"river"}[root.betting_round],
                "player": root.player,
                "pot":root.pot,
                "node_type":node_type
            }
        }

        if root.showdown == True:
            one_json["meta"]["payoffs"] = root.payoffs
        elif root.terminal == True:
            one_json["meta"]["payoff"] = root.payoff

        if root.children is not None:
            if node_type == "Chance":
                root_childs = [["dealcard",root]]
            else:
                root_childs = root.children.items()
            for one_action,one_child in root_childs:
                child_json = self.__gen_km_json(one_child,depth + 1,limit,root)
                if child_json:
                    children.append(child_json)
                    children_actions.append(one_action)

        if root.showdown == True and root.betting_round < 4:
            one_json["meta"]["node_type"] = node_type
            one_json["meta"]["round"] = {1:"preflop",2:"flop",3:"turn",4:"river"}[root.betting_round + 1]
            text = "Chance [DealCard]\nround:{}".format(one_json["meta"]["round"])
            one_json["data"]["text"] = text

            new_root = deepcopy(root)
            new_root.betting_round += 1
            child_json = self.__gen_km_json(new_root,depth + 1,limit,root)
            children.append(child_json)
            children_actions.append("dealcard")

        return one_json

class FiveCardTexasTreeBuilder(TreeBuilder):
    def build_tree(self):
        players = list(range(self.rule.players))
        player = 0
        root = ActionNode(None,committed = self.rule.get_beginning_chip(),players = players,player = player,last_action='begin',bet_history = [],betting_round=1)
        self.root = root
        return self.__build(root)

    def __build(self,root):
        #print("history:" + '|'.join(root.bet_history))
        if type(root) is ActionNode:
            self.build_action(root)
        elif type(root) is ShowdownNode:
            pass
        elif type(root) is TerminalNode:
            pass
        elif type(root) is DealCardNode:
            self.build_action(root)
        else:
            print(type(root),' should not be a node type')
            raise
        return root

    def get_possible_betting_sizes(self,root,player,next_player,typeofbet,rule):
        committed = root.committed
        if typeofbet == "bet":
            illegal_bets = self.rule.bet_sizes
        elif typeofbet == "raise":
            illegal_bets = self.rule.raise_sizes
        else:
            raise

        bets = []
        for one_bet in illegal_bets:
            assert('_pot' in one_bet or one_bet == "all-in")
            if '_pot' in one_bet:
                one_bet = one_bet.replace("_pot","")
                one_bet = float(one_bet)
            elif 'all-in' == one_bet:
                pass
            bets.append(one_bet)

        # 池里的数额实际上等于双方最小下注 * 2, 这里*2实际上是限定了玩家人数只能是2,多出来的那些是还没进池里的筹码(没有
        #     被call过)
        # pot = min(committed) * 2
        # 第二种计算方式，pod 值仅仅等于committed之和
        pot = max(committed) * 2

        # 根据conf计算下注数额，比如3x pot,2x pot, 0.5x pot
        possible_amounts = []

        def round_nearest(number,round_num):
            round_num = 1 / round_num
            return round((number * round_num)) / round_num
        sb = self.rule.small_blind
        bb = self.rule.big_blind

        for one_bet in bets:
            if type(one_bet) is float or type(one_bet) is int:
                if committed[player] == sb:
                    # 当德州扑克开始时，在第一个玩家动作时（sb位置玩家）,视作对手先下注一个bb,这个时候下注要扣除自己的sb
                    amount = one_bet * committed[next_player]  - sb
                    amount = round_nearest(amount,sb)
                elif committed[player] == bb and committed[next_player] == bb:
                    # 当德州扑克开始时，在第一个玩家call 的时候第二个玩家要 raise的时候,需要特殊处理
                    amount = one_bet * bb
                    amount = round_nearest(amount,sb)
                else:
                    # 其他时候，下注数额 总是和pot有关(比如2x pot,0.5x pot这种)
                    amount = one_bet * pot
                    amount = round_nearest(amount,bb)
                if typeofbet == "raise":
                    amount += (committed[next_player] - committed[player])
                if amount + committed[player] > rule.initial_effective_stack * rule.allin_threshold:
                    amount = -1
            elif type(one_bet) == str:
                assert(one_bet == 'all-in')
                amount = self.rule.stack - committed[player]
            if amount > 0:
                possible_amounts.append(amount)

        if committed[player] != sb: # 一开始的possible bet amount不能简单取整
            possible_amounts = [int(i) for i in possible_amounts if i > 0]
        if committed[player] == sb:
            possible_amounts = [i for i in possible_amounts if i >= bb]
        elif committed[player] == bb and committed[next_player] == bb:
            possible_amounts = [i for i in possible_amounts if i >= bb]
        else:
            # 如果不是开局情况，需要过滤掉一些过小的 amount
            gap = committed[next_player] - committed[player]
            assert(gap >= 0)
            possible_amounts = [i for i in possible_amounts if i >= gap * 2]

            # 同样，需要过滤掉一些过大的bet size
            possible_amounts = [i for i in possible_amounts if i <= self.rule.stack - committed[player]]

        return possible_amounts


    def build_action(self,root):
        # players list
        players = root.players

        # current players
        player = root.player

        # thenext_player possible actions after certain action
        if type(root) is DealCardNode:
            possible_actions = self.rule.legal_actions_after['roundbegin']
        else:
            possible_actions = self.rule.legal_actions_after[root.last_action.split('_')[0]]

        #print("possible_actions: ",possible_actions)
        # calculate the next player to make move
        next_player = (player + 1) % len(players)

        # for each possible action create a subtree
        if possible_actions is None:
            return

        # 如果双方都check了就停止生成动作
        check_limit = self.rule.check_limit
        # check numbers in a round cannot be larger than a certain number
        #checksum = sum([1 if i == 'check' else 0 for i in root.bet_history[-check_limit:] ])
        checksum = check_number_this_round(root)

        #flag = "".join(root.bet_history) == ''.join(['call', 'check', 'check', 'check', 'check', 'bet_1.0', 'call'])
        #if flag:
        #    print(root.bet_history,possible_actions)
        for one_action in possible_actions:
            if one_action == 'check':
                # 检查为什么DealCardNode 一个check就会进入下一个回合
                committed = deepcopy(root.committed)
                # 当不是第一轮的时候 call后面是不能跟check的
                if (root.last_action == 'call' and root.betting_round == 1) or checksum >= check_limit - 1:
                    # 双方均check的话,进入摊派阶段
                    # check 只有最有一轮（river）的时候才是摊派，否则都是应该进入下一轮的
                    if root.betting_round == self.rule.rounds:
                        nextnode = ShowdownNode(root,committed = committed,players = players,player = next_player,last_action=one_action,bet_history = root.bet_history + [one_action])
                    else:
                        # 第二轮开始先决定的就是deal card player了,所以这里的player 不是 next_player 而是player 1
                        nextnode = DealCardNode(root,committed = committed,players = players,player = 1,last_action=one_action,bet_history = root.bet_history + [one_action],betting_round=root.betting_round + 1)
                elif root.parent is not None:
                    nextnode = ActionNode(root,committed = committed,players = players,player = next_player,last_action=one_action,bet_history = root.bet_history + [one_action])
                else:
                    # 否则对方需要决定check之后要不要打或者盖牌什么的
                    nextnode = ActionNode(root,committed = committed,players = players,player = next_player,last_action=one_action,bet_history = root.bet_history + [one_action])
                self.__build(nextnode)
            elif one_action == 'bet':
                #  赌注大小分两类：固定size (比如10$)赌注和 pot 的固定倍数 的size (比如pot的50%，100%，300%)
                betting_sizes = self.get_possible_betting_sizes(root,player,next_player,"bet",self.rule)
                for one_betting_size in betting_sizes:
                    committed = deepcopy(root.committed)
                    #committed[player] += self.rule.amounts[one_action]
                    committed[player] += one_betting_size#self.rule.amounts[one_action]
                    one_action_bet = one_action + "_" + str(one_betting_size)
                    nextnode = ActionNode(root,committed = committed,players = players,player = next_player,last_action=one_action_bet,bet_history = root.bet_history + [one_action_bet])
                    self.__build(nextnode)

            elif one_action == 'call':
                committed = deepcopy(root.committed)
                # 不管是什么时候一方call了那么双方一定下注相同
                committed[player] += committed[next_player] - committed[player]
                # call之后应该进入下一轮了,如果是最后一轮就应该摊牌
                # 第一轮的call之后其实可以跟 raise 和check的，并且这个check会作为这一轮的结束
                if root.last_action == 'begin':
                    nextnode = ActionNode(root,committed = committed,players = players,player = next_player,last_action=one_action,bet_history = root.bet_history + [one_action])
                # 如果一方筹码用光了,或者轮数结束了，那么就该摊牌了
                elif root.betting_round == self.rule.rounds or np.any(self.rule.stack - np.asarray(root.committed) <= 0):
                    nextnode = ShowdownNode(root,committed = committed,players = players,player = next_player,last_action=one_action,bet_history = root.bet_history + [one_action])
                else:
                    nextnode = DealCardNode(root,committed = committed,players = players,player = 1,last_action=one_action,bet_history = root.bet_history + [one_action],betting_round = root.betting_round + 1)

                #nextnode = ShowdownNode(root,committed = committed,players = players,player = next_player,last_action=one_action,bet_history = root.bet_history + [one_action])
                self.__build(nextnode)
            elif one_action == 'raise':
                if root.last_action == 'call':
                    if root.parent and root.parent.parent is None:
                        pass
                    else:
                        continue
                # 第二轮之后的check后面只能跟 bet
                if root.last_action == 'check':
                    # TODO check这里的逻辑替换会不会引出新问题
                    #if root.parent and root.parent.parent is None:
                    if root.parent and (root.parent.parent is None and root.betting_round == 1):
                        pass
                    else:
                        continue

                # 如果raise次数超出限制，则不可以继续raise
                if raise_number_this_round(root) >= self.rule.raise_limit:
                    continue

                # 当第一个call 之后的raise需要特殊处理
                betting_sizes = self.get_possible_betting_sizes(root,player,next_player,"raise",self.rule)

                for one_betting_size in betting_sizes:
                    one_action_raise = one_action + "_" + str(one_betting_size)
                    committed = deepcopy(root.committed)
                    committed[player] += one_betting_size
                    nextnode = ActionNode(root,committed = committed,players = players,player = next_player,last_action=one_action_raise,bet_history = root.bet_history + [one_action_raise])
                    self.__build(nextnode)
            elif one_action  == 'fold':
                committed = deepcopy(root.committed)
                nextnode = TerminalNode(root,committed = committed,players = players,player = next_player,last_action=one_action,bet_history = root.bet_history + [one_action])
                self.__build(nextnode)
            else:
                raise

class PartGameTreeBuilder(FiveCardTexasTreeBuilder):
    def __init__(self,rule):
        self.rule = rule
        self.build_tree()
    def build_tree(self):
        players = list(range(self.rule.players))
        player = self.rule.current_player
        current_round = self.rule.current_round
        root = ActionNode(None,committed = self.rule.current_commit,players = players,player = player,last_action='roundbegin',bet_history = [],betting_round=current_round)
        self.root = root
        return self.__build(root)
    def __build(self,root):
        #print("history:" + '|'.join(root.bet_history))
        if type(root) is ActionNode:
            self.build_action(root)
        elif type(root) is ShowdownNode:
            pass
        elif type(root) is TerminalNode:
            pass
        elif type(root) is DealCardNode:
            self.build_action(root)
        else:
            print(type(root),' should not be a node type')
            raise
        return root


class Node(object):
    def __init__(self, parent, committed, players, player,  bet_history,betting_round=None,**kwargs):
        self.committed = deepcopy(committed)
        self.pot = sum(committed)
        self.bet_history = deepcopy(bet_history)
        self.players = players
        self.player = player
        self.children = None
        self.parent = parent

        #分别标记 是否到达终局的flag
        self.terminal = False
        self.showdown = False
        self.serialized = None

        # 第几轮下注，德州扑克中有四轮，preflop,flop , turn river，而其简化版--两张德州中仅有一轮
        self.betting_round = betting_round if betting_round is not None else (parent.betting_round if parent is not None else None)

    def get_opponent(self):
        if len(self.players) == 2:
            return (self.player + 1) % len(self.players)
        else:
            return list(set(self.players) - set([self.player,]))

    def add_child(self, child):
        if self.children is None:
            self.children = [child]
        else:
            self.children.append(child)

    def serialize(self):
        if self.serialized is None:
            self.serialized =  "[{}]".format("|".join(self.bet_history))
        return self.serialized

    def to_string(self):
        return "{}\n{} \n {}".format(
            "player: " + str(self.get_opponent()),
            "pot:" + "-".join([str(i) for i in self.committed]),
            hash(''.join(self.bet_history)) % 10000
        )

# TODO get chance node done later
class ChanceNode(Node):
    def __init__(self, parent, committed, players, player,  bet_history,betting_round=None,**kwargs):
        super().__init__(parent, committed, players, player,  bet_history,betting_round=betting_round)
        self.last_action = kwargs['last_action']
        if parent:
            self.parent.add_child(self,self.last_action)

    def add_child(self, child,action):
        if self.children is None:
            self.children = { action:child }
        else:
            self.children[action] = child

    def to_string(self):
        return "{}-{}\n{} \n {}".format(
            "player: " + str(self.get_opponent()),
            self.last_action,
            "pot:" + "-".join([str(i) for i in self.committed]),
            hash(''.join(self.bet_history)) % 10000
        )

class ActionNode(Node):
    def __init__(self, parent, committed, players, player,  bet_history,betting_round=None,**kwargs):
        super().__init__(parent, committed, players, player,  bet_history,betting_round=betting_round)
        self.last_action = kwargs['last_action']
        if parent:
            self.parent.add_child(self,self.last_action)

    def add_child(self, child,action):
        if self.children is None:
            self.children = { action:child }
        else:
            self.children[action] = child

    def to_string(self):
        return "{}-{}\n{} \n {}".format(
            "player: " + str(self.get_opponent()),
            self.last_action,
            "pot:" + "-".join([str(i) for i in self.committed]),
            hash(''.join(self.bet_history)) % 10000
        )

class ShowdownNode(ActionNode):
    def __init__(self, parent, committed, players, player,  bet_history,betting_round=None,**kwargs):
        super().__init__(parent, committed, players, player,  bet_history,betting_round=betting_round, ** kwargs)
        self.last_action = kwargs['last_action']
        if parent:
            self.parent.add_child(self,self.last_action)

        self.payoffs = {}
        # payoff's key is player id represent what happens if player i wins
        # it's value is payoffs for all players if player i wins

        for i in players:
            # everybody throw in money
            self.payoffs[i] = [-i for i in self.committed]
            # winner get all money
            self.payoffs[i][i] += self.pot

        # 仅考虑两个玩家的情况
        self.payoffs['tie'] = [-i + (self.pot / 2) for i in self.committed]
        self.showdown = True

    def add_child(self, child,action):
        # Showdown Node shouldn't have any child
        raise

    def to_string(self):
        return "{}\n{}-{}\n{} \n {} \n {}".format(
            "[++showdown++]",
            "player: " + str(self.get_opponent()),
            self.last_action,
            "pot:" + "-".join([str(i) for i in self.committed]),
            self.payoffs,
            hash(''.join(self.bet_history)) % 10000,
            )

class DealCardNode(ShowdownNode):
    def __init__(self, parent, committed, players, player,  bet_history,betting_round=None,**kwargs):
        super().__init__(parent, committed, players, player,  bet_history,betting_round=betting_round, ** kwargs)
        self.showdown = False

    def add_child(self, child,action):
        if self.children is None:
            self.children = { action:child }
        else:
            self.children[action] = child

    def to_string(self):
        return "{}\n{}-{}\n{} \n {} \n {}".format(
            "[++dealcard++]",
            "player: " + str(self.get_opponent()),
            self.last_action,
            "pot:" + "-".join([str(i) for i in self.committed]),
            self.payoffs,
            hash(''.join(self.bet_history)) % 10000,
            )


class TerminalNode(ActionNode):
    def __init__(self, parent, committed, players, player,  bet_history,betting_round=None,**kwargs):
        super().__init__(parent, committed, players, player,  bet_history,betting_round=betting_round, ** kwargs)
        self.last_action = kwargs['last_action']
        if parent:
            self.parent.add_child(self,self.last_action)

        self.payoff = []
        # payoff's key is player id represent what happens if player i wins
        # it's value is payoffs for all players if player i wins

        self.payoff = [-i for i in self.committed]
        # winner get all money
        self.payoff[player] += self.pot
        self.terminal = True

    def add_child(self, child,action):
        # Showdown Node shouldn't have any child
        raise

    def to_string(self):
        return "{}\n{}-{}\n{} \n {} \n {}".format(
            "[--terminal--]",
            "player: " + str(self.get_opponent()),
            self.last_action,
            "pot:" + "-".join([str(i) for i in self.committed]),
            self.payoff,
            hash(''.join(self.bet_history)) % 10000,
            )

class HolecardChanceNode(Node):
    def __init__(self, parent, committed, holecards, board, deck, bet_history, todeal):
        Node.__init__(self, parent, committed, holecards, board, deck, bet_history)
        self.todeal = todeal
        self.children = []

class BoardcardChanceNode(Node):
    def __init__(self, parent, committed, holecards, board, deck, bet_history, todeal):
        Node.__init__(self, parent, committed, holecards, board, deck, bet_history)
        self.todeal = todeal
        self.children = []
