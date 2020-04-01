curl  https://users.dcc.uchile.cl/~gnavarro/software/repair.tgz -o "repair.tgz"
mkdir repair
tar zxvf repair.tgz -C repair
cd repair/bal
make
cd ..
cd ..



repair/bal/repair escherichia_coli

repair/bal/repair cere

repair/bal/repair coreutils

repair/bal/repair einstein.en.txt

repair/bal/repair influenza

repair/bal/repair kernel

repair/bal/repair para
