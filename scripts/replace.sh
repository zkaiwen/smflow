#/bin/bash

echo "Replacing Inv_Gate"
find ../*.hpp -type f -print0 | xargs -0 sed -i 's/lx/llx/g'
