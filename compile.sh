mkdir process/build;
cd process/build; cmake .. && make ;

cd -;

mkdir render/build;
cd render/build; cmake .. && make ;

cd -;
