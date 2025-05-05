#include <iostream>
#include <string>
#include <vector>

using namespace std;

// Класс Занятие
class Lesson {
private:
    string name;
    string dayOfWeek;
    string time;

public:
    // Конструктор без параметров
    Lesson() : name(""), dayOfWeek(""), time("") {
        cout << "Вызван конструктор без параметров класса Lesson" << endl;
    }

    // Конструктор с параметрами
    Lesson(const string& n, const string& day, const string& t) 
        : name(n), dayOfWeek(day), time(t) {
        cout << "Вызван конструктор с параметрами класса Lesson (" << name << ")" << endl;
    }

    // Конструктор копирования
    Lesson(const Lesson& other) 
        : name(other.name), dayOfWeek(other.dayOfWeek), time(other.time) {
        cout << "Вызван конструктор копирования класса Lesson (" << name << ")" << endl;
    }

    // Деструктор
    ~Lesson() {
        cout << "Вызван деструктор класса Lesson (" << name << ")" << endl;
    }

    // Методы доступа
    string getName() const { return name; }
    string getDayOfWeek() const { return dayOfWeek; }
    string getTime() const { return time; }

    void printInfo() const {
        cout << "Занятие: " << name << ", День: " << dayOfWeek 
             << ", Время: " << time << endl;
    }
};

// Класс Расписание
class Schedule {
private:
    int schoolNumber;
    string address;
    vector<Lesson> lessons;

public:
    // Конструктор без параметров
    Schedule() : schoolNumber(0), address("") {
        cout << "Вызван конструктор без параметров класса Schedule" << endl;
    }

    // Конструктор с параметрами
    Schedule(int num, const string& addr) : schoolNumber(num), address(addr) {
        cout << "Вызван конструктор с параметрами класса Schedule (Школа №" 
             << schoolNumber << ")" << endl;
    }

    // Конструктор копирования
    Schedule(const Schedule& other) 
        : schoolNumber(other.schoolNumber), address(other.address), lessons(other.lessons) {
        cout << "Вызван конструктор копирования класса Schedule (Школа №" 
             << schoolNumber << ")" << endl;
    }

    // Деструктор
    ~Schedule() {
        cout << "Вызван деструктор класса Schedule (Школа №" 
             << schoolNumber << ")" << endl;
    }

    // Метод для добавления занятия (создает объект Lesson с параметрами)
    void addLesson(const string& name, const string& day, const string& time) {
        lessons.emplace_back(name, day, time);
    }

    // Метод для копирования занятия (создает указанное количество копий)
    void copyLessons(const Lesson& lesson, int count) {
        for (int i = 0; i < count; ++i) {
            lessons.push_back(Lesson(lesson));
        }
    }

    // Вывод информации о расписании
    void printSchedule() const {
        cout << "\nРасписание для школы №" << schoolNumber 
             << " по адресу: " << address << endl;
        cout << "Занятия:" << endl;
        for (const auto& lesson : lessons) {
            lesson.printInfo();
        }
        cout << endl;
    }
};

int main() {
    // Демонстрация конструкторов класса Lesson
    cout << "=== Демонстрация работы с классом Lesson ===" << endl;
    {
        Lesson lesson1; // конструктор без параметров
        Lesson lesson2("Математика", "Понедельник", "09:00"); // конструктор с параметрами
        Lesson lesson3(lesson2); // конструктор копирования
    } // здесь вызываются деструкторы
    
    cout << "\n=== Демонстрация работы с классом Schedule ===" << endl;
    {
        Schedule schedule1; // конструктор без параметров
        Schedule schedule2(123, "ул. Ленина, 10"); // конструктор с параметрами
        
        // Добавляем занятия с помощью конструктора с параметрами
        schedule2.addLesson("Физика", "Вторник", "10:30");
        schedule2.addLesson("Химия", "Среда", "11:45");
        
        // Создаем копии занятия
        Lesson mathLesson("Математика", "Пятница", "13:00");
        schedule2.copyLessons(mathLesson, 2);
        
        Schedule schedule3(schedule2); // конструктор копирования
        
        // Выводим расписания
        schedule2.printSchedule();
        schedule3.printSchedule();
    } // здесь вызываются деструкторы
    
    return 0;
}