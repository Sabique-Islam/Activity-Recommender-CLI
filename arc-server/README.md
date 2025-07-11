# Arc Server

## Overview →

C-based backend server to provide activity recommendations via a RESTful API.

## Folder Structure →

```
arc-server/
├── Dockerfile       
├── main.c             
├── Makefile           
├── README.md                        
├── cjson/             
│   ├── cJSON.c
│   └── cJSON.h
├── data/              
│   ├── activities.csv
│   └── glove.6B.300d.txt
├── include/           
│   ├── activities.h
│   ├── api.h
│   ├── common.h
│   ├── config.h
│   ├── embeddings.h
│   ├── http_server.h
│   ├── json_handler.h
│   ├── recommender.h
│   └── utils.h
├── src/               
│   ├── activities.c
│   ├── api.c
│   ├── config.c
│   ├── embeddings.c
│   ├── http_server.c
│   ├── json_handler.c
│   ├── recommender.c
│   ├── string_utils.c
│   └── utils.c
```
---

## Build →



---