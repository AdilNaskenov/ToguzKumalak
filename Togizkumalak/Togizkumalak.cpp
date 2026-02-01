#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <fstream>
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
	
public:
	Board() {
		for (int i = 0; i < 18; i++) pits[i] = 9;
	}
	bool isPitEmpty(int player, int choice) {
		int index = (player == 1) ? (choice - 1) : (choice - 1 + 9);
		return pits[index] == 0;
	}
	// Тот самый метод!
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
		if (currentPit <= 17 && currentPit >= 9) {
			if (pits[currentPit] % 2 == 0) {
				kazan1 += pits[currentPit];
				pits[currentPit] = 0;
				std::cout << "!!! Zahvat! Vi zabrali kamni protivnika !!!" << std::endl;
			}
		}
		
		// После хода всегда показываем результат
		show();

	}

	void makeMovePC(int choice) {
		// 1. Вычисляем индекс (для PC это 9-17)
		int startPit = (choice - 1) + 9;
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
		}
		show();
	}

	void show() {
		cout << "Ochco (PC): " << kazan2 << endl;

		for (int i = 9; i >= 1; i--) cout << i << "\t";
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



		for (int i = 1; i <= 9; i++) cout << i << "\t";
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
	game.show();
	// ВОТ ЗДЕСЬ мы вызываем метод!
	// Представь, что пользователь ввел число 3
	bool hod = true; // true - ты, false - PC
	while (true) {
		int choice;

		if (hod) {
			std::cout << "--- Vash hod! ---" << std::endl;
			std::cout << "Viberite lunku (1-9): ";
			std::cin >> choice;

			// Тут можно добавить проверку: если лунка пустая, попросить ввести еще раз
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
		
	}

	return 0;
}


















