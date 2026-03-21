WIP
# mini-redis

#Any file that has WIP on top of it, is WIP. Hopefully will be updated in due time + if i dont forget it. 

As the name has redis, it is indeed about redis. 
As the name has a "mini", its somewhat a mini version. 


WIP right now, the goal of this project is to enhance my understanding how redis works.Internally. (and some subquests like better-ing my C++, apart from the AlgoDS)

I dont have a structured guide right now, so I am just googling stuff, asking claude things. 
This is also my first attempt to stay away from any roadmaps, youtube playlists, and a try to come out of tutorial hell. I sincerely hope i can do this. 

I admit this may sound ironic ; how will I come out of tutorial hell, if I have claude. Well, for this project, I am limiting myself to 2 types of questions I can ask claude - the "what next?(s)", and the "whats important". Hoping that I reduce this "2" to 1 questions, in future projects (provided that I finish this :))

Steps to run this :- 

If you are a mac/linux user, claude/gpt this part. Apologies. Will update once I myself have the knowhow of this. 

For Windows 
1. Install WSL
2. run both below
```
sudo apt update && sudo apt upgrade -y

sudo apt install -y \
  build-essential \
  g++ \
  cmake \
  make \
  gdb \
  git \
  netcat-openbsd \
  redis-tools
```

## Build

```bash
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . -j"$(nproc)"
```

## Run

```bash
./build/mini-redis
```
