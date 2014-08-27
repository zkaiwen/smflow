#/bin/bash

echo "Replacing Inv_Gate"
find ../circuits/temp/ -type f -print0 | xargs -0 sed -i 's/Inv_gate/INV/g'

echo "Replacing Or_Gate and Nor_Gate"
find ../circuits/temp/ -type f -print0 | xargs -0 sed -i 's/Or_gate/OR/g'
find ../circuits/temp/ -type f -print0 | xargs -0 sed -i 's/Or3_gate/OR3/g'
find ../circuits/temp/ -type f -print0 | xargs -0 sed -i 's/Or4_gate/OR4/g'
find ../circuits/temp/ -type f -print0 | xargs -0 sed -i 's/Or5_gate/OR5/g'
find ../circuits/temp/ -type f -print0 | xargs -0 sed -i 's/Nor_gate/NOR/g'
find ../circuits/temp/ -type f -print0 | xargs -0 sed -i 's/Nor3_gate/NOR3/g'
find ../circuits/temp/ -type f -print0 | xargs -0 sed -i 's/Nor4_gate/NOR4/g'
find ../circuits/temp/ -type f -print0 | xargs -0 sed -i 's/Nor5_gate/NOR5/g'

echo "Replacing And_Gate and Nand_Gate"
find ../circuits/temp/ -type f -print0 | xargs -0 sed -i 's/And_gate/AND/g'
find ../circuits/temp/ -type f -print0 | xargs -0 sed -i 's/And3_gate/AND3/g'
find ../circuits/temp/ -type f -print0 | xargs -0 sed -i 's/And4_gate/AND4/g'
find ../circuits/temp/ -type f -print0 | xargs -0 sed -i 's/And5_gate/AND5/g'
find ../circuits/temp/ -type f -print0 | xargs -0 sed -i 's/Nand_gate/NAND/g'
find ../circuits/temp/ -type f -print0 | xargs -0 sed -i 's/Nand3_gate/NAND3/g'
find ../circuits/temp/ -type f -print0 | xargs -0 sed -i 's/Nand4_gate/NAND4/g'
find ../circuits/temp/ -type f -print0 | xargs -0 sed -i 's/Nand5_gate/NAND5/g'

echo "Replacing Flip_Flop_D_reset"
find ../circuits/temp/ -type f -print0 | xargs -0 sed -i 's/Flip_Flop_D_reset/FD/g'




echo "Replacing Inv_Gate CAP"
find ../circuits/temp/ -type f -print0 | xargs -0 sed -i 's/INV_GATE/INV/g'

echo "Replacing Or_Gate and Nor_Gate CAP"
find ../circuits/temp/ -type f -print0 | xargs -0 sed -i 's/OR_GATE/OR/g'
find ../circuits/temp/ -type f -print0 | xargs -0 sed -i 's/OR3_GATE/OR3/g'
find ../circuits/temp/ -type f -print0 | xargs -0 sed -i 's/OR4_GATE/OR4/g'
find ../circuits/temp/ -type f -print0 | xargs -0 sed -i 's/OR5_GATE/OR5/g'
find ../circuits/temp/ -type f -print0 | xargs -0 sed -i 's/NOR_GATE/NOR/g'
find ../circuits/temp/ -type f -print0 | xargs -0 sed -i 's/NOR3_GATE/NOR3/g'
find ../circuits/temp/ -type f -print0 | xargs -0 sed -i 's/NOR4_GATE/NOR4/g'
find ../circuits/temp/ -type f -print0 | xargs -0 sed -i 's/NOR5_GATE/NOR5/g'

echo "Replacing And_Gate and Nand_Gate CAP"
find ../circuits/temp/ -type f -print0 | xargs -0 sed -i 's/AND_GATE/AND/g'
find ../circuits/temp/ -type f -print0 | xargs -0 sed -i 's/AND3_GATE/AND3/g'
find ../circuits/temp/ -type f -print0 | xargs -0 sed -i 's/AND4_GATE/AND4/g'
find ../circuits/temp/ -type f -print0 | xargs -0 sed -i 's/AND5_GATE/AND5/g'
find ../circuits/temp/ -type f -print0 | xargs -0 sed -i 's/NAND_GATE/NAND/g'
find ../circuits/temp/ -type f -print0 | xargs -0 sed -i 's/NAND3_GATE/NAND3/g'
find ../circuits/temp/ -type f -print0 | xargs -0 sed -i 's/NAND4_GATE/NAND4/g'
find ../circuits/temp/ -type f -print0 | xargs -0 sed -i 's/NAND5_GATE/NAND5/g'

echo "Replacing Flip_Flop_D_reset CAP"
find ../circuits/temp/ -type f -print0 | xargs -0 sed -i 's/FLIP_FLOP_D_RESET/FD/g'

echo "Renaming Inputs"
find ../circuits/temp/*og* -type f -print0 | xargs -0 sed -i 's/I1/I0/g'
find ../circuits/temp/*og* -type f -print0 | xargs -0 sed -i 's/I2/I1/g'
find ../circuits/temp/*og* -type f -print0 | xargs -0 sed -i 's/I3/I2/g'
find ../circuits/temp/*og* -type f -print0 | xargs -0 sed -i 's/I4/I3/g'
find ../circuits/temp/*og* -type f -print0 | xargs -0 sed -i 's/I5/I4/g'
find ../circuits/temp/*og* -type f -print0 | xargs -0 sed -i 's/I6/I5/g'


echo "Replacing Logics with VCC and GND"
find ../circuits/temp/*og* -type f -print0 | xargs -0 sed -i 's/logic_0/GND/g'
find ../circuits/temp/*og* -type f -print0 | xargs -0 sed -i 's/logic_1/VCC/g'

