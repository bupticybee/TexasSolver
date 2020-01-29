from jpype import *
import yaml
import numpy as np
import sys
sys.path.append("resources")
from python.TreeBuilder import *

with open('resources/yamls/general_rule.yaml') as fhdl:
    conf = yaml.load(fhdl)

startJVM(getDefaultJVMPath(), "-ea", "-Djava.class.path=%s" % "./RiverSolver.jar")

PokerSolver = JClass('icybee.riversolver.runtime.PokerSolver')

ps = PokerSolver("Dic5Compairer",
                 "./resources/compairer/card5_dic_sorted_shortdeck.txt",
                 376993,
                 ['A', 'K', 'Q', 'J', 'T', '9', '8', '7', '6'],
                 ['h', 's', 'd', 'c']
                )

rule = RulesBuilder(
    conf,
    current_commit = [2,2],
    current_round =  2,
    raise_limit = 1,
    check_limit = 2,
    small_blind = 0.5,
    big_blind = 1,
    stack = 10,
    bet_sizes = ["1_pot"],
)


gameTree = PartGameTreeBuilder(rule)


depth = np.inf
json = gameTree.gen_km_json("./.tree.km".format(depth),limit=depth,ret_json=True)

#ps.build_game_tree("./resources/gametree/part_tree_turn_withallin.km")
#ps.build_game_tree("./resources/gametree/game_tree_flop.km")
#ps.build_game_tree("./resources/gametree/part_tree_depthinf.km")
ps.build_game_tree("./.tree.km")

ps.train(
    "AA,KK,QQ,JJ,TT,99,88,77,66,AK,AQ,AJ,AT,A9,A8,A7,A6,KQ,KJ,KT,K9,K8,K7,K6,QJ,QT,Q9,Q8,Q7,Q6,JT,J9,J8,J7,J6,T9,T8,T7,T6,98,97,96,87,86,76",
    "AA,KK,QQ,JJ,TT,99,88,77,66,AK,AQ,AJ,AT,A9,A8,A7,A6,KQ,KJ,KT,K9,K8,K7,K6,QJ,QT,Q9,Q8,Q7,Q6,JT,J9,J8,J7,J6,T9,T8,T7,T6,98,97,96,87,86,76",
    #"Kd,Jd,Td,7s,8s",
    #"Kd,Jd,Td,7s",
    "Kd,Jd,Td",
    200, # iterations
    50, # print_interval
    False, # debug
    True, # parallel
    "outputs_strategy.json",
    "log.txt",
    "discounted_cfr",
    "none",
    -1, # threads
    1, # action fork prob
    1, # chance fork prob
    1, # fork every tree depth
    4, # fork minimal size
)