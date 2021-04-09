import yaml
import numpy as np
import sys
sys.path.append("resources")
from python.TreeBuilder import *
import bindSolver

with open('general_rule.yaml') as fhdl:
    conf = yaml.load(fhdl)

ps = bindSolver.PokerSolver("A,K,Q,J,T,9,8,7,6","h,s,d,c","card5_dic_sorted_shortdeck.txt",376993)

rule = RulesBuilder(
    conf,
    current_commit = [5,5],
    current_round = 2,
    raise_limit = 2,
    check_limit = 2,
    small_blind = 0.5,
    big_blind = 1,
    stack = 100,
    bet_sizes = ["0.5_pot","1_pot","all-in"],
    raise_sizes = ["1_pot","2_pot","all-in"],
)

gameTree = PartGameTreeBuilder(rule)


depth = np.inf
json = gameTree.gen_km_json("./.tree.km".format(depth),limit=depth,ret_json=True)

#ps.build_game_tree("./resources/gametree/part_tree_turn_withallin.km")
#ps.build_game_tree("./resources/gametree/game_tree_flop.km")
#ps.build_game_tree("./resources/gametree/part_tree_depthinf.km")
ps.load_game_tree("./.tree.km")

ps.train(
    "AA,KK,QQ,JJ,TT,99,88,77,66,AK,AQ,AJ,AT,A9,A8,A7,A6,KQ,KJ,KT,K9,K8,K7,K6,QJ,QT,Q9,Q8,Q7,Q6,JT,J9,J8,J7,J6,T9,T8,T7,T6,98,97,96,87,86,76",
    "AA,KK,QQ,JJ,TT,99,88,77,66,AK,AQ,AJ,AT,A9,A8,A7,A6,KQ,KJ,KT,K9,K8,K7,K6,QJ,QT,Q9,Q8,Q7,Q6,JT,J9,J8,J7,J6,T9,T8,T7,T6,98,97,96,87,86,76",
    #"Kd,Jd,Td,7s,8s",
    "Kd,Jd,Td,7s",
    #"Kd,Jd,Td",
    "log.txt",
    100, # iterations
    10, # print_interval
    "discounted_cfr", #algorithm
    -1 # threads
)
ps.dump_strategy("dumped_strategy.json")