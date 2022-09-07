mkdir "The Witcher"
cd "The Witcher"
mkdir "Geralt"
mkdir "Zoltan & Dandelion"
mkdir "Triss & Yennifer"

cd "Geralt"
truncate -s 1024 Geralt.txt

cd ../"Zoltan & Dandelion"
echo "Я думал, тут ждут меня: горячий окорок и холодное пиво, а тут жопа!" > Zoltan.txt
echo "Людям для жизни необходимы три вещи: еда, питье и сплетни." > Dandelion.txt

cd ../"Triss & Yennifer"
ln ../Geralt/Geralt.txt Yennifer.txt
ln -s ../Geralt/Geralt.txt Triss.txt
