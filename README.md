# Push-Relabel: алгоритм проталкивания предпотока

Реализация и сравнение трёх алгоритмов поиска максимального потока:
- **Dinic** (KACTL)
- **Push-Relabel (HLPP)** (KACTL, эвристики Highest-Label + Gap)
- **BasePR** (собственная упрощённая версия для обучения)

## Сборка и запуск бенчмарков

```bash
g++ -std=c++17 -O2 bench.cpp -o bench
./bench
```

## Структура репозитория

- `PushRelabelBase.hpp` — базовая реализация
- `Dinic.h`, `PushRelabel.h` — эталонные реализации из библиотеки KACTL
- `bench.cpp` — бенчмарки (5 различных графов, медиана)
- `benchmark_results.csv` — сырые результаты замеров
- `benchmark_results.png` — итоговый график сравнения
- `slides/` — исходники презентации (LaTeX) и PDF

## Результаты

На плотных графах (плотность 0.7–0.9) HLPP стабильно обгоняет Dinic, что согласуется с теоретической оценкой \(O(V^2 \sqrt{E})\).

## Ссылки

- [CP-Algorithms: Push-Relabel](https://cp-algorithms.com/graph/push-relabel.html)
- [НИУ ИТМО](https://neerc.ifmo.ru/wiki/index.php?title=Алгоритм_проталкивания_предпотока)
- [UT Austin](https://iss.oden.utexas.edu/?p=projects/galois/analytics/preflow_push)


## Автор

Антонов Кирилл, Б01-411
