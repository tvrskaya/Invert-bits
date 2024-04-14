#include <iostream>
#include <bitset>
#include <random>

using std::cout;

uint64_t* GenerateArray_64(size_t size)
{
    uint64_t* array = new uint64_t[size];
    std::mt19937_64 generator(11);
    std::uniform_int_distribution<uint64_t> disrt(0u, std::numeric_limits<uint64_t>::max());
    for (int i = 0; i < size; i++)
    {
        array[i] = disrt(generator);
    }
    return array;
}

unsigned int* GenerateArray_uint(size_t size)
{
    unsigned int* array = new unsigned int[size];
    std::mt19937 generator(11);
    std::uniform_int_distribution<unsigned int> disrt(0u, std::numeric_limits<unsigned int>::max());
    for (int i = 0; i < size; i++)
    {
        array[i] = disrt(generator);
    }
    return array;
}

template <class T>
void PrintArrayBits(T array, size_t size)
{
    for (int i = 0; i < size; i++)
    {
        cout << std::bitset<64>(array[i]) << "\n";
    }
}

template <class T>
void PrintArray(T array, size_t size)
{
    for (int i = 0; i < size; i++)
    {
        cout << array[i] << "\n";
    }
}

template <class T>
void InvertBitsInValue(T x, size_t begin, size_t lenght)
{
    const size_t SizeInBits = sizeof(*x)*8; //Работаем в предположении о том, что Т - указатель
    uint64_t mask = 0xffffffffffffffff;
    cout << "Begin: " << begin << "\n";
    cout << "Lenght: " << lenght << "\n";
    mask >>= (64 - lenght); 
    mask <<= (SizeInBits - (begin + lenght)); 
    cout << "m:  " << std::bitset<64>(mask) << "\n";
    cout << "v:  " << std::bitset<64>(*x) << "\n";
    cout << "mv: " << std::bitset<64>(*x ^ mask) << "\n";
    *x = *x ^ mask;
}


//Допустим, что begin и lenght измеряются в битах
//arraySize - количество элементов в массиве
template <class T>
void InsertBitsInArray(T array, size_t arraySize, size_t begin, size_t lenght)
{
    size_t SizeInBits = sizeof(array[0])*8; //Размер элемента массива в битах
    if (arraySize*SizeInBits < begin + lenght)
    {
        cout << "Array not so long\n";
        return;
    }
    //Берем указатель на начало массива
    T p = array;
    //Вычисляем, сколько шагов нужно сделать от первого элемента
    unsigned int steps = begin/SizeInBits; //Кол-во шагов

    if (steps > 0)
    {
        p += steps;
        begin -= steps * SizeInBits; //Вычитаем из начала количество шагов в битах
    }

    //Проверяем теперь, выходит ли begin+lenght на следующий элемент массива
    size_t difference = 0;
    if (begin + lenght > SizeInBits)
    {
        difference = (begin + lenght) - SizeInBits; //Длина инвертирования у следующего элемента массива
    }

    if (difference == 0)
    {
        InvertBitsInValue(p, begin, lenght);
        cout << "nv: " << *p << "\n";
    }
    else 
    {
        //Если difference > SizeInBits, то получаем ситуацию, когда нужно перемещаться на следующие элементы
        //Инвертируем биты от begin и до конца 
        InvertBitsInValue(p, begin, SizeInBits - begin);
        while (difference > SizeInBits)
        {
            difference -= SizeInBits;
            p++; //Перемещаемся на следующий элемент
            if (difference > SizeInBits) //Обрабатываем ситуацию, когда придется еще раз перемещатся на следующий элемент
            {
                InvertBitsInValue(p, 0, SizeInBits);//И инвертируем весь элемент массива
            }
            else 
            {
                InvertBitsInValue(p, 0, difference); //В противном случае инвертируем только часть
            }
            cout << "nv: " << *p << "\n";
        }
        
    }
}

int main(void)
{
    cout << "m - Mask\n" << "v - Value\n" << "mv - Masked Value" << "nv - New Value\n";

    cout << "Invert in unsigned int array\n";
    auto array = GenerateArray_uint(10);
    InsertBitsInArray(array, 10, 4, 80);

    //Проверка работы на 8 битах
    cout << "Invert 8 vit value\n";
    uint8_t x = 0b1010100;
    InvertBitsInValue(&x, 3, 2);
    cout << "nv: " << std::bitset<64>(x) << "\n";

    //Проверка защиты от неверной длины
    cout << "Check if array not so long\n";
    InsertBitsInArray(array, 10, 4, 80000);

    delete[] array;
    return 0;
}