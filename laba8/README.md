# Библиотека BST Containers

Этот проект предоставляет три шаблонных класса двоичных деревьев поиска (BST) с различными способами обхода:

- **`InOrderIterator`**  
  Обход «in-order» (симметричный). Элементы посещаются в порядке возрастания (или по заданному компаратору).

- **`PreOrderIterator`**  
  Обход «pre-order» (прямой). Сначала посещается узел, затем левое и правое поддеревья.

- **`PostOrderIterator`**  
  Обход «post-order» (обратный). Сначала левое и правое поддеревья, затем узел.

Каждый контейнер параметризуется:

- `T` — тип хранимых элементов.  
- `Compare` — функтор сравнения (по умолчанию `std::less<T>`).  
- `Allocator` — тип аллокатора (по умолчанию `std::allocator<T>`).

Все три контейнера удовлетворяют STL-требованиям:

- **Container**  
- **AssociativeContainer**  
- **ReversibleContainer** (для in-order и post-order)  
- **AllocatorAwareContainer**  
- **BidirectionalIterator**

Контейнеры предоставляют:

- Конструирование/деструктуру  
- `insert(const T&) → std::pair<iterator, bool>`  
- `find(const T&)`, `count(const T&)`  
- `size()`, `empty()`, `clear()`  
- Прямые итераторы: `begin()`, `end()`  
- Обратные итераторы: `rbegin()`, `rend()`


---

## Тестирование

Юнит-тесты реализованы с помощью Google Test в файле `tests/test_bst_containers.cpp`. Они проверяют:

- Типовые требования и соответствие STL-контейнеру.  
- Базовые операции: `insert`, `find`, `count`, `clear`.  
- Корректность последовательностей обхода для in-, pre- и post-order.  




