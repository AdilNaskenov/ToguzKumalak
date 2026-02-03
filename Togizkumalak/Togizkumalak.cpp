#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <fstream>
#include <thread>
#include <chrono>
#include <random>

using namespace std;

const string RED = "\033[31m";
const string GREEN = "\033[32m";
const string RESET = "\033[0m";
const string BLUE = "\033[34m";

int randomInt(int from, int to) {
	static std::random_device rd;      // источник энтропии
	static std::mt19937 gen(rd());      // генератор
	std::uniform_int_distribution<> dist(from, to);

	return dist(gen);
}

class Player {
private:
	string name;
	int rating;
	int ratingPC;
public:
	Player() {
		ifstream readName("name");
		ifstream readElo("ELOhu");
		ifstream readElopc("ELOpc");
		if (readName.is_open() && readElo.is_open() && readElopc.is_open()){
			readElo >> rating;
			readElopc >> ratingPC;
			getline(readName, name);
			cout << "Welcom back!: " << name << "\nYour ELO: " << rating << endl;
			cout << "ELO PC: " << ratingPC << endl;
			readName.close();
			readElo.close();
			readElopc.close();
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
	bool tyzdyk = false;
	bool tyzdyk2 = false;
	int g2;
	int g;
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
		if (kazan1 > 81 || kazan2 > 81) {
			double S, S1;
			if (kazan1 > kazan2) {
				cout << "Vi pobedili! Pozdravlyayu!\n";
				S = 1; 
				S1 = 0;
			}
			else if (kazan2 > kazan1) {
				cout << "PC pobedil! Vsego horoshego!\n";
				S = 0;
				S1 = 1;
			}
			else {
				cout << "Nichya! Horoshaya igra!\n";
				S = 0.5;
				S1 = 0.5;
			}
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
		int currentPit = (startPit + 1) % 18;
		// Правило Тогыз Кумалак:
		
		if (hand == 1) {
			pits[startPit] = 0;
			pits[currentPit]++;
			if (pits[currentPit] % 2 == 0 && currentPit == 9) {
				kazan1 += pits[currentPit];
				pits[currentPit] = 0;
				cout << GREEN << "!!! Zahvat! Vi zabrali kamni protivnika !!!" << RESET << endl;
			}
			if (!tyzdyk && pits[currentPit] == 3) {
				kazan1 += 3;
				tyzdyk = true;
				pits[currentPit] = 0;
				g = currentPit;
				cout <<  GREEN << "!!! Tyzdyk! Vi zabrali 3 kamnya !!!" << RESET << endl;
			}
		}
		else pits[startPit] = 1; // Один оставляем
		int stonesToDistribute = hand - 1;

		for (int i = 0; i < stonesToDistribute; i++) {
			pits[currentPit]++;
			currentPit = (currentPit + 1) % 18; // Вот так мы ходим по кругу бесконечно
		}
	
		currentPit--;

		if (currentPit <= 17 && currentPit >= 9) {
			if (pits[currentPit] % 2 == 0) {
				kazan1 += pits[currentPit];
				pits[currentPit] = 0;
				std::cout << GREEN <<  "!!! Zahvat! Vi zabrali kamni protivnika !!!" << RESET << std::endl;
			}
			if (!tyzdyk && pits[currentPit] == 3) {
				kazan1 += 3;
				tyzdyk = true;
				pits[currentPit] = 0;
				g = currentPit;
				std::cout << GREEN << "!!! Tyzdyk! Vi zabrali 3 kamnya !!!"<< RESET << std::endl;
			}
		}
		if (tyzdyk) {
			kazan1 += pits[g];
			pits[g] = 0;
		}
		show(choice,-1);

	}

	void makeMovePC(int pcChoice) {
		// 1. Вычисляем индекс (для PC это 9-17)
		int startPit = (pcChoice - 1) + 9;
		int hand = pits[startPit];

		if (hand == 0) return; // Компьютер просто не должен выбирать пустые лунки

		int currentPit = (startPit + 1) % 18;
		

		if (hand == 1) {
			pits[startPit] = 0;
			pits[currentPit]++;
			if (pits[currentPit] % 2 == 0 && currentPit == 18) {
				kazan2 += pits[currentPit]; // В КАЗАН КОМПЬЮТЕРА
				pits[currentPit] = 0;
				std::cout << RED << "PC zabral vashi kamni!" << RESET << std::endl;
			}
			if (!tyzdyk2 && pits[currentPit] == 3) {
				kazan2 += 3;
				tyzdyk2 = true;
				g2 = currentPit;
				std::cout << RED << "Tyzdyk. PC zabral 3 kamnya!"<< RESET << std::endl;
			}
		}
		else pits[startPit] = 1;

		int stonesToDistribute = hand - 1;

		for (int i = 0; i < stonesToDistribute; i++) {
			pits[currentPit]++;
			currentPit = (currentPit + 1) % 18;
		}
		currentPit--;
		
		// 2. Логика захвата для PC (на стороне игрока 0-8)
		if (currentPit >= 0 && currentPit <= 8) {
			if (pits[currentPit] % 2 == 0) {
				kazan2 += pits[currentPit]; // В КАЗАН КОМПЬЮТЕРА
				pits[currentPit] = 0;
				std::cout << RED << "PC zabral vashi kamni!" << RESET << std::endl;
			}
			if (!tyzdyk2 && pits[currentPit] == 3) {
				kazan2 += 3;
				tyzdyk2 = true;
				g2 = currentPit;
				std::cout << RED << "Tyzdyk PC zabrali 3 kamnya!" << RESET << std::endl;
			}
		}
		if (tyzdyk2) {
			kazan2 += pits[g2];
			pits[g2] = 0;
		}
		
	}

	void show(int choice,int pcChoice) {
		cout << "Ochco (PC): " <<  BLUE << kazan2 << RESET << endl;

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

		cout << "Ochco (You): " << BLUE << kazan1 << RESET << endl << "--------------------------" << endl;
	}

	int PC1() {
		int bestChoice = -1;
		int maxAdvantage = -1000; // Начальное очень маленькое число

		// Проверяем каждую лунку компьютера (индексы 9-17)
		for (int s = 9; s < 18; s++) {
			if (pits[s] == 0) continue; // Пропускаем пустые

			// --- СИМУЛЯЦИЯ ХОДА КОМПЬЮТЕРА ---
			int temppits[18];
			for (int i = 0; i < 18; i++) temppits[i] = pits[i];

			int tempKazan2 = 0;
			int hand = temppits[s];
			int currentPit;

			// Сама логика хода (упрощенно для симуляции)
			if (hand == 1) {
				temppits[s] = 0;
				currentPit = (s + 1) % 18;
				temppits[currentPit]++;
			}
			else {
				temppits[s] = 1;
				int stones = hand - 1;
				currentPit = (s + 1) % 18;
				for (int i = 0; i < stones; i++) {
					temppits[currentPit]++;
					currentPit = (currentPit + 1) % 18;
				}
				currentPit = (currentPit - 1 + 18) % 18;
			}

			// Проверка захвата для PC
			if (currentPit >= 0 && currentPit <= 8) {
				if (temppits[currentPit] % 2 == 0) {
					tempKazan2 = temppits[currentPit];
				}
			}

			// --- ОЦЕНКА ОТВЕТА ИГРОКА ---
			int maxPlayerGain = 0;
			for (int h = 0; h < 9; h++) { // Игрок может ответить любой из 9 лунок
				if (temppits[h] == 0) continue;

				// Считаем, сколько игрок может забрать в лучшем случае
				int testHand = temppits[h];
				int finalPit = (h + testHand) % 18; // Грубая прикидка финиша
				if (finalPit >= 9 && finalPit <= 17) {
					if ((temppits[finalPit] + 1) % 2 == 0) {
						if (temppits[finalPit] + 1 > maxPlayerGain)
							maxPlayerGain = temppits[finalPit] + 1;
					}
				}
			}

			// Вычисляем выгоду: мой захват минус лучший возможный захват игрока
			int advantage = tempKazan2 - maxPlayerGain;

			if (advantage > maxAdvantage) {
				maxAdvantage = advantage;
				bestChoice = (s - 9) + 1; // Переводим индекс обратно в 1-9
			}
		}

		// Если ничего умного не нашли, просто идем первой непустой лункой
		if (bestChoice == -1) {
			for (int i = 9; i < 18; i++) {
				if (pits[i] > 0) return (i - 9) + 1;
			}
		}

		return bestChoice;
	}
};

// 2. А здесь начинается сама игра
int main() {
	// Создаем объекты (те самые "шаблоны" или "красные кнопки")
	
	Board game;
	
	// Показываем начальную доску
	game.show(-1,-1);
	// ВОТ ЗДЕСЬ мы вызываем метод!
	// Представь, что пользователь ввел число 3
	bool hod = true; // true - ты, false - PC
	while (game.winner()) {
		int choice;

		if (hod) {
			cout << "--- Vash hod! ---" << std::endl;
			cout << "Viberite lunku (1-9): ";
			/*choice = randomInt(1, 9);*/

			cin.clear();
			/*cin.ignore(numeric_limits<streamsize>::max(), '\n');*/

			cout << "Vy vybrali: " << choice << endl;

			game.makeMove(choice);

			hod = false; // Передаем ход компьютеру
		}
		else {
			std::cout << "--- Hod PC... ---" << std::endl;

			// Логика выбора для PC (как мы обсуждали раньше)
			int pcChoice = 1;
			do {
				pcChoice = game.PC1();
			} while (game.isPitEmpty(2, pcChoice)); // Тебе нужно добавить этот метод в класс

			game.makeMovePC(pcChoice);

			hod = true; // Возвращаем ход тебе
		}
		/*this_thread::sleep_for(chrono::seconds());*/
	}
	Player p1;
	return 0;
}





































































