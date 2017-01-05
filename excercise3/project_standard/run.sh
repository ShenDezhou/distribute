#!/bin/bash
java  -cp  lib/odps-sdk-mapred-0.17.8-jar-with-dependencies.jar:target/mapreduce-1.0-SNAPSHOT.jar com.aliyun.odps.mapred.WordCount src.csv dest.csv
echo 'job done'