#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstdlib>
#include <time.h>
#include <algorithm>
#include <chrono>

using namespace std;


struct element
{   
    int weight;
    int value;
};

struct input_data
{
    int capacity;
    vector<element> set;
};

vector<element> output;



int goal_fuction(vector<element> solution){
    int total_value = 0;
    for (element e : solution) total_value += e.value;
    return total_value;
}


vector<element> load_file(string file_name){
    vector<element> result = {};
    ifstream file(file_name);
    int w, v;

    while(file >> w >> v){
        result.push_back({w,v});
    }

    return result;
}


void save_file(string file_name, vector<element> solution, int time){
    ofstream file;
    file.open(file_name);
    for (element e : solution){
        file << e.weight << " " << e.value <<endl;
    }
    file << "Wartość funkcji celu: " << goal_fuction(solution) <<endl;
    file << "Czas obliczeń: " << time <<"[ms]";
    file.close();
}


void generate_random_problem_to_file(string file_name, int problem_size, int random_range){
    ofstream myfile;
    int weight, value;
    myfile.open(file_name);
    for (int i=0; i < problem_size; i++){
        weight = rand() % random_range;
        value = rand() % random_range;
        myfile << weight;
        myfile << " ";
        myfile << value;
        myfile <<"\n";
    }
    myfile.close();
}


void print_input(input_data input){
    cout<<endl<<"Wczytany zbiór: {";
    for (element e : input.set){
        cout<<"{"<<e.weight<<","<<e.value<<"},";
    }
    cout<<"}"<<endl<<"Wczytana pojemność: "<<input.capacity<<endl<<endl;
}


int total_weight(vector<element> solution){
    int total_weight = 0;
    for (element e : solution) total_weight += e.weight;
    return total_weight;
}


void print_solution(vector<element> solution){
    cout<<"Rozwiązanie: {";
    for (element e : solution){
        cout<<"{"<<e.weight<<","<<e.value<<"},";
    }
    cout<<"}   Całkowita waga: "<<total_weight(solution)<<"   "<<"Całkowita wartość: "<<goal_fuction(solution)<<endl<<endl;
}


void print_combinations(vector<vector<element>> combinations){
    for (vector<element> combination: combinations){
        cout<<"{"<<endl;
        for(element e : combination){
            cout<<"{"<<e.weight<<","<<e.value<<"},";
        }
        cout<<"}"<<endl;
    }
    cout<<"Liczba kombinacji: "<<combinations.size()<<endl;
}


vector<element> generate_random_domain_point(input_data input, int mode){
    vector<element> input_set = input.set;
    vector<element> solution;
    int total_weight = 0;
    int index;
    
    switch (mode){
        case 1:
            while (!input_set.empty()){
                
                index = rand() % input_set.size();

                if (input_set[index].weight + total_weight <= input.capacity){
                    
                    total_weight += input_set[index].weight;
                    solution.push_back(input_set[index]);
                    swap(input_set[index], input_set.back());
                    input_set.pop_back();
                }
                else input_set.pop_back();
            }
            break;
        case 2:
            for (element e : input_set){
                if (total_weight + e.weight <= input.capacity){
                    total_weight += e.weight;
                    solution.push_back(e);
                }
            }
            break;
        case 3:
            sort(input_set.begin(), input_set.end(), [](element a, element b){return a.weight < b.weight;});
            for (element e : input_set){
                if (total_weight + e.weight <= input.capacity){
                    total_weight += e.weight;
                    solution.push_back(e);
                }
            }
            break;
        default:
            break;
    }
    return solution;
}


void combination_rec(int offset, int elements_number, input_data &problem, vector<element> &combination, vector<element> &best_found){
    if (elements_number == 0) {
        if (goal_fuction(combination) > goal_fuction(best_found) && total_weight(combination) <= problem.capacity) best_found = combination;
        return;
    }

    for (int i=offset; i <= problem.set.size() - elements_number; ++i) {
        combination.push_back(problem.set[i]);
        combination_rec(i+1, elements_number-1, problem, combination, best_found);
        combination.pop_back();
    }
}


vector<element> next_solution(input_data problem, vector<element> solution){
    vector<element> best_found = solution;
    vector<element> combination;

    for (int i = 1; i <= problem.set.size(); i++) combination_rec(0, i, problem, combination, best_found);

    return best_found;
}


int main(int argc, char** argv){

    input_data input;
    srand(time(NULL));
    input.set = load_file(argv[1]);
    input.capacity = stoi(argv[2]);
    string output_name = argv[3];

    print_input(input);

    vector<element> random_s = generate_random_domain_point(input, 1);

    print_solution(random_s);

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    vector<element> next_s = next_solution(input, random_s);

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    int time = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    print_solution(next_s);

    cout << "Czas obliczeń = " << time << "[ms]" <<endl;

    save_file(output_name, next_s, time);

    return 0;
}