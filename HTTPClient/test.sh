#!/bin/bash

for i in {1..10000}
do
  curl http://121.14.39.195:18080/ > /dev/null
done
