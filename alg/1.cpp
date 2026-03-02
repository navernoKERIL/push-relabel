#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

// Вычисляет числовой код паттерна (0 = '-', 1 ='?', 2 = '#')
// для пары guess и secret. Используется для фильтрации и подсчёта энтропии.
int get_pattern_code(const std::string& guess, const std::string& secret, int L) 
{
    std::vector<int> count(26, 0);
    for (char c : secret)
        count[c - 'a']++;

    int code = 0;
    int mul = 1;

    // 1. Сначала размечаем '#' и уменьшаем счётчик оставшихся вхождений
    for (int i = 0; i < L; i++){
        if (guess[i] == secret[i]){
            code += 2 * mul;
            count[guess[i] - 'a']--;
        }
        mul *= 3;
    }

    // 2. Теперь '?' в позициях что мы не учли выше
    mul = 1;
    for (int i = 0; i < L; i++){
        char g = guess[i];
        if (guess[i] != secret[i]){  // только если позиция не отмечена '#'
            if (count[g - 'a'] > 0){
                code += 1 * mul;
                count[g - 'a']--;
            }
        }
        mul *= 3;
    }

    return code;
}

// Преобразует строку ответа от системы (# ? -) в числовой код (0..3^L-1)
// Это нужно, чтобы сравнивать с кодами, полученными от get_pattern_code
int get_response_code(const std::string& response, int L) 
{
    if (static_cast<int>(response.size()) != L)
        return -1;  // ошибка ввода

    int code = 0;
    int multiplier = 1;
    for (int i = 0; i < L; i++){
        char r = response[i];
        if (r == '#'){
            code += 2 * multiplier;
        } else if (r == '?'){
            code += 1 * multiplier;
        } // '-' остаётся 0
        multiplier *= 3;
    }
    return code;
}

// Выбирает лучшее слово по максимальной энтропии
// На первой попытке — просто первое слово из словаря
// На следующих — перебираем possible + немного слов из начала словаря
std::string choose_best_entropy(const std::vector<std::string>& possible,
                                const std::vector<std::string>& dict,
                                int L,
                                int attempt)
{
    if (possible.empty()) return "";
    if (possible.size() == 1) return possible[0];

    // Быстрый выход на последних ходах
    if (attempt >= 1 && possible.size() <= 6)
        return possible[0];
    

    // Первая попытка — лучшее по количеству уникальных букв из первых 2100
    if (attempt == 0){
        std::string best;
        int max_unique = -1;

        size_t limit = std::min(dict.size(), size_t(2100));
        for (size_t i = 0; i < limit; ++i){
            const auto& w = dict[i];
            bool seen[26] = {};
            int uniq = 0;
            for (char c : w){
                int idx = c - 'a';
                if (!seen[idx]){
                    seen[idx] = true;
                    uniq++;
                }
            }
            if (uniq > max_unique || (uniq == max_unique && (best.empty() || w < best))){
                max_unique = uniq;
                best = w;
            }
        }
        return best.empty() ? dict[0] : best;
    }

    // Остальные попытки — как было, только possible
    std::string best_guess;
    double best_entropy = -1e9;

    int MAX_PATTERNS = 1;
    for (int i = 0; i < L; ++i) MAX_PATTERNS *= 3;

    for (const auto& g : possible){
        std::vector<int> pattern_count(MAX_PATTERNS, 0);

        for (const auto& s : possible){
            int pat = get_pattern_code(g, s, L);
            pattern_count[pat]++;
        }

        double entropy = 0.0;
        int total = static_cast<int>(possible.size());
        for (int cnt : pattern_count){
            if (cnt == 0) continue;
            double p = static_cast<double>(cnt) / total;
            entropy -= p * std::log2(p);
        }

        if (entropy > best_entropy + 1e-9){
            best_entropy = entropy;
            best_guess = g;
        }
        else if (std::abs(entropy - best_entropy) < 1e-9){
            if (best_guess.empty() || g < best_guess){
                best_guess = g;
            }
        }
    }

    return best_guess.empty() ? possible[0] : best_guess;
}

int main() 
{
    int N, M, L;
    std::cin >> N >> M >> L;

    std::vector<std::string> dict(N);
    for (int i = 0; i < N; i++)
        std::cin >> dict[i];

    // Возможные загаданные слова — только из первых ≤2100
    int max_secret = std::min(N, 2100);
    std::string win(L, '#');

    for (int round = 0; round < M; round++){
        // Начинаем каждый раунд с полного списка возможных ответов
        std::vector<std::string> possible;
        possible.reserve(max_secret);
        for (int i = 0; i < max_secret; i++)
            possible.push_back(dict[i]);

        for (int attempt = 0; attempt < 6; attempt++){
            // Выбираем лучшее слово по энтропии
            std::string guess = choose_best_entropy(possible, dict, L, attempt);

            // Делаем догадку
            std::cout << guess << std::endl;

            // Получаем ответ от системы
            std::string response;
            std::cin >> response;

            // Победа — выходим из цикла попыток
            if (response == win) break;

            // Получаем числовой код ответа
            int resp_code = get_response_code(response, L);
            if (resp_code < 0) break;

            // Фильтруем список возможных слов
            std::vector<std::string> new_possible;
            new_possible.reserve(possible.size() / 2 + 1);

            for (const auto& s : possible){
                if (get_pattern_code(guess, s, L) == resp_code) 
                    new_possible.push_back(s);
            }

            possible = std::move(new_possible);

            // Если ничего не осталось — прерываем (ошибка логики)
            if (possible.empty()) break;
        }
    }
    return 0;
}