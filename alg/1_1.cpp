#include <iostream>
#include <string>
#include <vector>

int main() {
    std::cout << "Начало main\n";
    std::cout.flush();

    int N, M, L;
    std::cin >> N >> M >> L;

    std::cout << "Прочитали N=" << N << " M=" << M << " L=" << L << "\n";
    std::cout.flush();

    std::vector<std::string> dict(N);
    std::cout << "Создали vector на " << N << " элементов\n";
    std::cout.flush();

    for (int i = 0; i < N; ++i) {
        std::cin >> dict[i];
        std::cout << "Прочитали слово " << (i+1) << "/" << N << ": " << dict[i] << "\n";
        std::cout.flush();
    }

    std::cout << "Все слова прочитаны. Словарь размером " << dict.size() << "\n";
    std::cout.flush();

    std::string dummy;
    std::cin >> dummy;  // ждём raise
    std::cout << "Прочитали первый ответ: " << dummy << "\n";

    std::cout << "Программа дошла до конца без падения\n";
    return 0;
}