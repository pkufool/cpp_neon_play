## Cpp neon play

A cpp arm neon playground for beginners. 

* A simple editor developed with [flask](https://github.com/pallets/flask) and [codemirror](https://codemirror.net/) with which one can run neon instrucments any time and any where with a browser only.

* Plenty of examples that help you to learn arm SIMD from scratch。

### Structure
```bash
.
├── README.md
├── codelab
│   ├── codelab.txt               # example list
│   ├── neon_product.cc           # example code
│   └── neon_sum.cc
├── neon_service.py               # editor backend service
├── requirements.txt
├── run_service.sh                # start script
├── static
│   ├── codemirror
│   ├── fonts
│   ├── index.css
│   ├── index.js
│   ├── jquery-3.3.1.min.js
│   ├── main.js                   # editor frontend 
│   └── vue.js
└── templates
    └── index.html                # editor page
```

### Set up your own browser editor
To set up your own C/S architecture editor, you need a server runs on arm cpu that with python installed.
* Install requirements
```
pip install -r requirements.txt
```
* Run server
```
bash run_service.sh
```
You can custom your server with: 
```
--port      # http listen port
--memory    # max memory usage for single program(in bytes)
--timeout   # max time execute for single program(in seconds)
```

**if you don't have a arm cpu, feel free to use the one deployed on my cell phone (http://neon.kingway.fun/)**

### Examples
All the examples are listed in the codelab folder with `.cc` extension, `codelab.txt` is a text file lists the file names and display labels.
* [Array Sum]()
* [Dot Product]()
* [Maximum Element]()
* [Minimum Element]()
* [AddVecVec]()

