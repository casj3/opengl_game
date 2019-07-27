#pragma once



template<typename T>
void AddElement(T* elements, T element, unsigned int* amount)
{
	elements[amount] = element;

	amount++;
}

template<typename T>
void RemoveElement(T* elements, unsigned int index, unsigned int* amount)
{
	amount--;
	if (index != amount)
	{
		elements[index] = elements[amount];
	}
	else
	{
		elements[index] = elements[index - 1];
	}
}

template<typename T>
void ResizeArray(T** elements, unsigned int* arraySize, unsigned int numAddedElements)
{
	T* oldArray = elements;
	elements = (T*)malloc(sizeof(T)* (arraySize + numAddedElements));

	for (unsigned int i = 0; i < arraySize; i++)
	{
		elements[i] = oldArray[i];
	}

	arraySize += numAddedElements;

	free(oldArray);
}
