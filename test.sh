sudo apt-get update
sudo apt-get install -y g++ make zip
make
mv ./app ./app
zip -r app.zip app ./SteamAPI/linux64/libsteam_api.so
