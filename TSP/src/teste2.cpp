// #include <bits/stdc++.h>

// using namespace std;

// int main(){
//     vector<int> teste = {1, 2, 3, 4, 5, 6, 7, 1};
    
//     auto print = [&](vector<int> t) -> void {
//         for (int i = 0; i < t.size(); i++){
//             cout << t[i] << " \n"[i == t.size()-1];
//         }
//         cout << "\n";
//     };

//     // print();

//     // // 1 2 3 4 5 6 7 1
//     // // 1 5 6 2 3 4 7 1
//     // // 1 5 6 4 2 3 7 1

//     // // i: [1, 2]
//     // // j: [4, 5]
//     int i_l = 1, i_r = 3;
//     int j_l = 4, j_r = 6;

//     // if (i_r + 1 == j_l) {
//     //     // do nothing
//     // }
//     // else rotate(teste.begin() + i_l, teste.begin() + i_r + 1, teste.begin() + j_l);

//     // print();

//     // // adiciona o comprimento?
    
//     // // i_l += (j_r - j_l + 1);
//     // // i_r += (j_r - j_l + 1);

//     // rotate(teste.begin() + i_l, teste.begin() + j_l, teste.begin() + j_r + 1);

//     vector<int> aux;
//     aux.reserve(teste.size());

//     // inicio até i_l
//     aux.insert(aux.end(), teste.begin(), teste.begin() + i_l);

//     // j_l até j_r
//     aux.insert(aux.end(), teste.begin() + j_l, teste.begin() + j_r + 1);

//     // o que esta entre i_r até j_l
//     aux.insert(aux.end(), teste.begin() + i_r + 1, teste.begin() + j_l);

//     // i_l até i_r
//     aux.insert(aux.end(), teste.begin() + i_l, teste.begin() + i_r + 1);

//     // j_r até o final
//     aux.insert(aux.end(), teste.begin() + j_r + 1, teste.end());

//     print(aux);

// }