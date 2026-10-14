#include <iostream>
#include <string>
#include <cstdio>

class Student
{

public:
    Student(const std::string &name, int age, double avgrade)
        : name(name), age(age), avgrade(avgrade) {}
    void display() const
    {
        printf("Name: %s, Age: %d, Avg.: %.2f\n", name.c_str(), age, avgrade);
    }

private:
    std::string name;
    int age;
    double avgrade;
};
int main()
{
    Student student1("Alice", 20, 3.8);
    Student student2("Bob", 21, 3.6);
    puts("Student 1 details:");
    student1.display();
    puts("Student 2 details:");
    student2.display();
    return 0;
}