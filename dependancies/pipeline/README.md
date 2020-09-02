# AnyOpt

AnyOpt (Any Optional) is a C++11 compatible library

this header-only library was primarily developed due to the requirements of my resource manager:
* being able to store any type    
* being able to automatically free the resource stored upon object death
* being able to determine if anything is actually stored or not, since nullptr is a valid storage

# cloning

## as a non-submodule

```shell script
git clone --recursive https://github.com/mgood7123/AnyOpt
```

## as a submodule

```shell script
git submodule add https://github.com/mgood7123/AnyOpt path/where/AnyOpt/should/reside/AnyOpt
git submodule update --init --recursive
```

# building and testing

```shell script
make test_debug
```