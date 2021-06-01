import yaml
import numpy as np
import sys
sys.path.append("resources")
from python.TreeBuilder import *

with open('general_rule.yaml') as fhdl:
    conf = yaml.load(fhdl)


rule = RulesBuilder(
    conf,
    current_commit = [5,5],
    current_round = 2,
    raise_limit = 2,
    check_limit = 2,
    small_blind = 0.5,
    big_blind = 1,
    stack = 100,
    #bet_sizes = ["0.3_pot","0.6_pot","1_pot","2_pot","all-in"],
    bet_sizes = ["0.5_pot","1_pot","all-in"],
    raise_sizes = ["1_pot","2_pot","all-in"],
)

gameTree = PartGameTreeBuilder(rule)


depth = np.inf
json = gameTree.gen_km_json("tree_new.km".format(depth),limit=depth,ret_json=True)