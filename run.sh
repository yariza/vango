mkdir assets/canvas;

for name in "$@"; do

    echo;
    echo $name;

    cd process/build;
    ./process -i "../../assets/input_images/"$name".png" -s "../../assets/styles/"$name".yaml" -o "../../assets/canvas/"$name".yaml";

    cd -;

    cd render/build;
    ./render "../../assets/canvas/"$name".yaml" -s "../../assets/styles/"$name".yaml" -o "../../results/"$name".png" -d 0;

    cd -;
done
