mingw32-make

k=1

for instance in ./instances/*; do
    echo $instance >> ./output.txt
    
    echo "Instance $k of 8"
    for strategy in DFS BFS BBS; do
        echo "Processando $instance com $strategy"
        
        printf "%s: " "$strategy" >> ./output.txt
        ./tsp ${instance} $strategy >> ./output.txt
    done
    k=$(($k+1))
done

echo "-" >> ./output.txt