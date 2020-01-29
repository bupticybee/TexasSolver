java -jar RiverSolver.jar -c \
"resources/yamls/rule_shortdeck_turnriversolver.yaml" \
-p1 \
"AA,KK,QQ,JJ,TT,99,88,77,66,AK,AQ,AJ,AT,A9,A8,A7,A6,KQ,KJ,KT,K9,K8,K7,K6,QJ,QT,Q9,Q8,Q7,Q6,JT,J9,J8,J7,J6,T9,T8,T7,T6,98,97,96,87,86,76" \
-p2 \
"AA,KK,QQ,JJ,TT,99,88,77,66,AK,AQ,AJ,AT,A9,A8,A7,A6,KQ,KJ,KT,K9,K8,K7,K6,QJ,QT,Q9,Q8,Q7,Q6,JT,J9,J8,J7,J6,T9,T8,T7,T6,98,97,96,87,86,76" \
-b \
"Kd,Jd,Td,7s,8s" \
-n \
100 \
-i \
10 \
-d \
false \
-p \
true \
-o \
"resources/outputs/outputs_strategy.json" \
-l \
"resources/outputs/outputs_log.txt" \
-a \
"discounted_cfr" \
-m \
"none" \
-t \
2