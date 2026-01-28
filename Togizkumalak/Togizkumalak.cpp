#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <fstream>


using namespace std;

// Константы цветов
const string RED = "\033[31m";
const string GREEN = "\033[32m";
const string RESET = "\033[0m";

// Функция для отрисовки доски (чтобы не повторять код)
void printBoard(int kazan[18], int score1, int score2) {
    system("cls");
    cout << "Ochco (PC): " << score2 << endl;

    // Верхний ряд (Игрок 2 / PC)
    for (int i = 9; i >= 1; i--) cout << i << "\t";
    cout << endl;
    for (int i = 17; i >= 9; i--) cout << GREEN << kazan[i] << RESET << "\t";
    cout << endl << endl;

    // Нижний ряд (Игрок 1 / Ты)
    for (int i = 0; i < 9; i++) cout << GREEN << kazan[i] << RESET << "\t";
    cout << endl;
    for (int i = 1; i <= 9; i++) cout << i << "\t";
    cout << endl;

    cout << "Ochco (You): " << score1 << endl << "--------------------------" << endl;
}

int main() {
    while (true) {
        int first = 0, second = 0;
        int kazan[18];
        for (int i = 0; i < 18; i++) kazan[i] = 9; // Инициализация

        bool tyzgik_exists = true; // Можно ли еще создать туздык
        int tslot = -1;
        srand(time(0));
        bool hod = true; // true - твой ход, false - PC




        while (true) {
            printBoard(kazan, first, second);
            int n;
            // Логика выбора лунки
            if (hod) {
                do { n = 1 + rand() % 9; } while (kazan[n - 1] == 0);
             /*   do { cin >> n; } while (kazan[n - 1] == 0);*/
                cout << "You choose: " << n << endl;
            }
            else {
                // Ход PC
                int lunka[9];  
                for (int j = 0; j < 9; j++) lunka[j] = 0;  

                for (int i = 10; i <= 18; i++) {
                    int a = (kazan[i - 1] + i - 1) - 18;
                    if (kazan[a - 1] > 0 && (kazan[a - 1] + 2) % 3 == 0) {
                        lunka[i - 10] = kazan[a - 1];
                    }
                }

                int more = 0;
                for (int i = 1; i <= 9; i++) {
                    if (lunka[i - 1] > more) {
                        more = lunka[i - 1];
                    }
                }

                do { n = 10 + rand() % 9; } while (kazan[n - 1] == 0);

                for (int i = 0; i <= 8; i++) {
                    if (kazan[i] == more) {  
                        n = i - 1;
                    }
                }
               
                cout << "PC chooses: " << n << endl;
            }

            int index = n - 1;
            int balls = kazan[index];
           

            kazan[index] = (balls == 1) ? 0 : 1; // Если 1 шарик, он уходит в след. лунку. Если больше - один остается.
            int balls_to_distribute = (balls == 1) ? 1 : balls - 1;

            int step = index + 1;
            int last_pos = index;

            for (int i = 0; i < balls_to_distribute; i++) {
                if (step > 17) step = 0;
                kazan[step]++;
                last_pos = step;
                step++;
            }

            // Правила захвата (Четность)
            if (hod && last_pos >= 9 && last_pos <= 17) {
                if (kazan[last_pos] % 2 == 0) {
                    first += kazan[last_pos];
                    kazan[last_pos] = 0;
                }
                else if (tyzgik_exists && kazan[last_pos] == 3 && last_pos != 17) {
                    // Туздык (нельзя в последней лунке)
                    tslot = last_pos;
                    tyzgik_exists = false;
                    first += 3;
                    kazan[last_pos] = 0;
                }
            }
            else if (!hod && last_pos >= 0 && last_pos <= 8) {
                if (kazan[last_pos] % 2 == 0) {
                    second += kazan[last_pos];
                    kazan[last_pos] = 0;
                }
                else if (tyzgik_exists && kazan[last_pos] == 3 && last_pos != 8) {
                    tslot = last_pos;
                    tyzgik_exists = false;
                    second += 3;
                    kazan[last_pos] = 0;
                }
            }

            // Сбор из Туздыка (если он есть)
            if (tslot != -1) {
                if (tslot >= 9 && tslot <= 17) { first += kazan[tslot]; kazan[tslot] = 0; }
                else { second += kazan[tslot]; kazan[tslot] = 0; }
            }

            // Проверка победы
            if (first > 81 || second > 81) break;

            hod = !hod;
            /*cin.get();*/ // Пауза до нажатия Enter
        }


        // Посчет ЭЛО
        string ELOhu, ELOpc;

        ifstream readELOhu("ELOhu"), readELOpc("ELOpc");
        if (readELOhu.is_open() && readELOpc.is_open()) {
            getline(readELOhu, ELOhu);
            getline(readELOpc, ELOpc);
            readELOhu.close();
            readELOpc.close();
        }
        else {
            ofstream ELOhu1("ELOhu"), ELOpc1("ELOpc");
            if (ELOhu1.is_open() && ELOpc1.is_open()) {
                ELOhu1 << 400;
                ELOpc1 << 400;
                ELOhu1.close();
                ELOpc1.close();
            }
        }


        int ratingHu = stoi(ELOhu);
        int ratingPc = stoi(ELOpc);

        double S, S1;
        if (first > 81) { S = 1; S1 = 0; }
        else if (second > 81) { S = 0; S1 = 1; }
        else S = 0.5;
        // рейтинг человека
        double E = 1.0 / (1.0 + pow(10.0, (double)(ratingPc - ratingHu) / 400.0));
        int R = ratingHu + 20 * (S - E);

        // рейтинг компа
        double E1 = 1.0 / (1.0 + pow(10.0, (double)(ratingHu - ratingPc) / 400.0));
        int R1 = ratingPc + 20 * (S1 - E1);

        ofstream FileELOhu("ELOhu"), FileELOpc("ELOpc");
        if (FileELOhu.is_open() && FileELOpc.is_open()) {
            FileELOhu << R;
            FileELOpc << R1;
            FileELOpc.close();
            FileELOhu.close();
        }

        cout << (first > 81 ? "YOU WIN!" : "PC WINS!") << endl;
        cout << "Human rating: " << R << endl << "PC rating: " << R1 << endl;
        cin.ignore(1000, '\n');
    }
    return 0;
}






