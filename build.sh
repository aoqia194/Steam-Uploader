g++ -g $(find ./src -name '*.cpp') \
-o ./app \
-I. \
-L./SteamAPI/linux64 -lsteam_api \
-pthread