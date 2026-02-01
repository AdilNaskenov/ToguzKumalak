#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <fstream>
#include <thread>
#include <chrono>

using namespace std;

const string RED = "\033[31m";
const string GREEN = "\033[32m";
const string RESET = "\033[0m";

class Player {
private:
	string name;
	int rating;

public:
	Player() {
		ifstream readName("name");
		ifstream readElo("ELOhu");
		if (readName.is_open() && readElo.is_open()) {
			readElo >> rating;
			getline(readName, name);
			cout << "Welcom back!: " << name << "\nYour ELO: " << rating << endl;
			readName.close();
			readElo.close();
		}
		else {
			cout << "Choose your name: ";
			cin >> name;
			rating = 400;
			ofstream Name("name");
			ofstream Elo("ELOhu");
			if (Name.is_open() && Elo.is_open()) {
				Name << name;
				Elo << rating;
			}
			Name.close();
			Name.close();
		}
	}
};

class Board {
private:
	int pits[18];
	int kazan1 = 0;
	int kazan2 = 0;
	bool tyzdyk = true;
public:
	Board() {
		for (int i = 0; i < 18; i++) pits[i] = 9;
	}
	bool isPitEmpty(int player, int choice) {
		int index = (player == 1) ? (choice - 1) : (choice - 1 + 9);
		return pits[index] == 0;
	}
	// Тот самый метод!
	bool winner() {
		int sum1 = 0;
		for (int i = 0; i < 9; i++) {
			if (pits[i] == 0) sum1++;
		}
		if (sum1 == 9) {
			cout << "PC pobedil! Vsego horoshego!\n";
			return false;
		}

		int sum2 = 0;
		for (int i = 9; i < 18; i++) {
			if (pits[i] == 0) sum2++;
		}
		if (sum2 == 9) {
			cout << "Vi pobedili! Pozdravlyayu!\n";
			return false;
		}
		if (kazan1 > 81 || kazan2 > 81) {
			if (kazan1 > kazan2) {
				cout << "Vi pobedili! Pozdravlyayu!\n";
			}
			else if (kazan2 > kazan1) {
				cout << "PC pobedil! Vsego horoshego!\n";
			}
			else cout << "Nichya! Horoshaya igra!\n";
			return false;
		}
		
		return true;      
	}

	
	void makeMove(int choice) {
		int startPit = choice - 1; // Переводим ввод игрока (1-9) в индекс (0-8)
		int hand = pits[startPit];

		if (hand == 0) {
			std::cout << "Oshibka! Lunka pusta." << std::endl;
			return;
		}

		// Правило Тогыз Кумалак:
		if (hand == 1) {
			pits[startPit] = 0;
			int nextPit = (startPit + 1) % 18;
			pits[nextPit]++;
		}
		else pits[startPit] = 1; // Один оставляем
		int stonesToDistribute = hand - 1;

		int currentPit = (startPit + 1) % 18;

		for (int i = 0; i < stonesToDistribute; i++) {
			pits[currentPit]++;
			currentPit = (currentPit + 1) % 18; // Вот так мы ходим по кругу бесконечно
		}
		currentPit--;
		if (currentPit <= 17 && currentPit >= 9) {
			if (pits[currentPit] % 2 == 0) {
				kazan1 += pits[currentPit];
				pits[currentPit] = 0;
				std::cout << "!!! Zahvat! Vi zabrali kamni protivnika !!!" << std::endl;
			}
			if (tyzdyk && pits[currentPit] == 3) {
				kazan1 += 3;
				tyzdyk = false;
				pits[currentPit] = 0;
				std::cout << "!!! Tyzdyk! Vi zabrali 3 kamnya !!!" << std::endl;
			}
		}
		
		show(choice,-1);

	}

	void makeMovePC(int pcChoice) {
		// 1. Вычисляем индекс (для PC это 9-17)
		int startPit = (pcChoice - 1) + 9;
		int hand = pits[startPit];

		if (hand == 0) return; // Компьютер просто не должен выбирать пустые лунки

		int currentPit; // Объявляем снаружи, чтобы видеть её после цикла

		if (hand == 1) {
			pits[startPit] = 0;
			currentPit = (startPit + 1) % 18;
			pits[currentPit]++;
		}
		else pits[startPit] = 1;

		int stonesToDistribute = hand - 1;
		currentPit = (startPit + 1) % 18;

		for (int i = 0; i < stonesToDistribute; i++) {
			pits[currentPit]++;
			currentPit = (currentPit + 1) % 18;
		}
		// 2. Логика захвата для PC (на стороне игрока 0-8)
		if (currentPit >= 0 && currentPit <= 8) {
			if (pits[currentPit] % 2 == 0) {
				kazan2 += pits[currentPit]; // В КАЗАН КОМПЬЮТЕРА
				pits[currentPit] = 0;
				std::cout << "PC zabral vashi kamni!" << std::endl;
			}
			if (tyzdyk && pits[currentPit] == 3) {
				kazan2 += 3;
				tyzdyk = false;
				std::cout << "!!! Tyzdyk! Vi zabrali 3 kamnya !!!" << std::endl;
			}
		}
		show(-1,pcChoice);
	}

	void show(int choice,int pcChoice) {
		cout << "Ochco (PC): " << kazan2 << endl;

		for (int i = 9; i >= 1; i--) {
			if (i == pcChoice) cout << RED << i << RESET << "\t";
			else cout << i << "\t";
		}
		cout << endl;



		// Она видит pits напрямую, не нужно его передавать!
		int temp[9];

		for (int i = 17; i >= 9; i--) {
			int a = -1 * (i - 17);
			temp[a] = pits[i];
		}
		while (true) {
			int error = 0;
			for (int i = 0; i < 9; i++) {
				if (temp[i] == 0) {
					error++;
					cout << " \t";
				}
				else if (temp[i] == 1) {
					cout << "*\t";
					temp[i] = 0;
					error++;
				}
				else {
					cout << "**\t";
					temp[i] -= 2;
				}
			}
			if (error == 9) break;
			cout << endl;
		}
		cout << endl << endl;
		// Доска
		int sum = 0;
		for (int i = 0; i < 9; i++) {
			temp[i] = pits[i];
			if (temp[i] > sum) sum = temp[i];
		}
		int end = (sum++) / 2;

		for (int j = 0; j <= end; j++) {
			int sum = 0;
			for (int i = 0; i < 9; i++) {
				if (temp[i] > sum) sum = temp[i];
			}

			if (sum % 2 == 0) sum--;

			for (int i = 0; i < 9; i++) {
				if (temp[i] == 0) cout << " \t";
				else if (temp[i] >= sum) {
					if (temp[i] % 2 == 0) {
						cout << "**\t";
						temp[i] -= 2;
					}
					else if (temp[i] % 2 == 1) {
						cout << "*\t";
						temp[i]--;
					}
				}
				else cout << " \t";
			}
			cout << endl;
		}


		cout << endl;



		for (int i = 1; i <= 9; i++) {
			if (i == choice) cout << GREEN << i << RESET << "\t";
			else cout << i << "\t";
		}
		cout << endl;

		cout << "Ochco (You): " << kazan1 << endl << "--------------------------" << endl;
	}
};

// 2. А здесь начинается сама игра
int main() {
	// Создаем объекты (те самые "шаблоны" или "красные кнопки")
	Player p1;
	Board game;
	
	// Показываем начальную доску
	game.show(-1,-1);
	// ВОТ ЗДЕСЬ мы вызываем метод!
	// Представь, что пользователь ввел число 3
	bool hod = true; // true - ты, false - PC
	while (game.winner()) {
		int choice;

		if (hod) {
			string cmd;

			while (true) {
				cout << "--- Vash hod! ---" << std::endl;
				cout << "Viberite lunku (1-9): ";
				cin >> cmd;
				if (cmd.size() == 1 && isdigit(cmd[0])) {
					choice = cmd[0] - '0'; 
					break; 
				}

				cout << "Oshibka! Tolko odna cifra.\n";

				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
			}

			cout << "Vy vybrali: " << choice << endl;

			game.makeMove(choice);

			hod = false; // Передаем ход компьютеру
		}
		else {
			std::cout << "--- Hod PC... ---" << std::endl;

			// Логика выбора для PC (как мы обсуждали раньше)
			int pcChoice;
			do {
				pcChoice = (rand() % 9) + 1;
			} while (game.isPitEmpty(2, pcChoice)); // Тебе нужно добавить этот метод в класс

			game.makeMovePC(pcChoice);

			hod = true; // Возвращаем ход тебе
		}
		this_thread::sleep_for(chrono::seconds(2));
	}

	return 0;
}



































