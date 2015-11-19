#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

#define HASHSIZE 4001 //Make sure to change this appropriately for your document size. Array size will be HASHSIZE - 1
#define ARRAY_SIZE 23868

using namespace std;

// Use folding on a string, summed 4 bytes at a time, This will be the index of the array where we will insert
int sfold(const char* key)
{
	unsigned int *lkey = (unsigned int *)key;
	int intlength = strlen(key)/4;
	unsigned int sum = 0;

	for(int i=0; i<intlength; i++)
		sum += lkey[i];
    // Now deal with the extra chars at the end
	int extra = strlen(key) - intlength*4;
	char temp[4];
	lkey = (unsigned int *)temp;
	lkey[0] = 0;
	for(int i=0; i<extra; i++)
		temp[i] = key[intlength*4+i];
	sum += lkey[0];
	return sum % HASHSIZE;
}

struct Node //The structure used in this project
{
	string word;
	int frequency;
};

void swap(Node a[], int i, int j)
{
	 Node temp;
     temp.word = a[j].word;
     temp.frequency = a[j].frequency;
     a[j].word = a[i].word;
     a[j].frequency = a[i].frequency;
     a[i].word = temp.word;
     a[i].frequency = temp.frequency;
}

// Quicksort is used to find the most frequently used words.

int partition (Node a[], int lo, int hi)
{
	int i = lo; int j = hi + 1;
	while (true)
	{
		while (a[++i].frequency < a[lo].frequency)
			if (i==hi) break;
		while (a[lo].frequency < a[--j].frequency)
			if (j==lo) break;
		if (i>=j) break;
		swap (a,i,j);
	}
	swap (a, lo, j);
	return j;
}

void quicksortE (Node a[], int lo, int hi)
{
	if (hi<=lo) return;
	int j = partition (a, lo, hi);
	quicksortE (a, lo, j-1);
	quicksortE (a, j+1, hi);
}

void quicksort (Node a[])
{
     int lo = 0; int hi = HASHSIZE -1;
     quicksortE(a, lo, hi);
}

class HashArray
{
	private:
		int size;
		int accesses;
		int current;

	public:

		Node hasharr[HASHSIZE];
		int updates[HASHSIZE];
		int cost[HASHSIZE];

		HashArray()
		{
			size = 0;
			for(int i = 0; i < HASHSIZE; i++)
			{
				hasharr[i].frequency = 0;
				hasharr[i].word = "NULL";
				updates[i] = 0;
				cost[i] = 0;
			}

		}
		~HashArray() {}

		void reset()
		{
			current = -1;
		}

		void update(string charkey) // Main function for inputing a value into the hash array.
		{
			accesses = 0; //to count the probes
			const char* temp = charkey.c_str();
			int intkey = sfold(temp);
			int temp1 = size;
			if (hasharr[intkey].frequency == 0)
				{
					hasharr[intkey].word = charkey;
					hasharr[intkey].frequency = 1;
					size++;
					accesses++;
				}
			else if(hasharr[intkey].word == charkey)
				{
					hasharr[intkey].frequency++;
					accesses++;
				}
			else
				{
					while(hasharr[intkey].frequency != 0 && hasharr[intkey].word != charkey)
					{
						accesses++;
					    intkey++;
					    if(intkey > 4000)
					        intkey = 0;
					}

					if(hasharr[intkey].word == charkey)
					{
						hasharr[intkey].frequency++;
						accesses++;
					}
					else
					{
					hasharr[intkey].word = charkey;
					hasharr[intkey].frequency++;
					size++;
					}
				}

				if(size == temp1 + 1)
				{
					cost[size] = cost[size-1] + accesses;
					updates[size] = updates[size-1] + 1;
				}
				else
				{
					cost[size] += 1;
					updates[size] += 1; //update of both statistial arrays.
				}
		}

		int hashsize()//Number of entries in the hash array.
		{
            return size;
		}

		void find1timewords() // prints all words that appear only once and then prints the amount of these types of words.
		{
			int i = 0;
			int count = 0;
			while(i < HASHSIZE)
			{
				if(hasharr[i].frequency == 1)
				{
					cout << hasharr[i].word<<" ";
					count++;
				}
				++i;
			}
			cout << "\nNumber of words appearing one time: " << count<<endl;
		}

		void printBackN(int n) // Prints the last N hash entries at indexes 0 thru N. Because sorted least to greatest.
		{
			for (int i = 1; i < n + 1; i++)
				cout << "\nword: '" << hasharr[HASHSIZE-i].word << "'    frequency: " << hasharr[HASHSIZE-i].frequency;
            cout << endl;
		}

};

void getratios(int a[], int b[], int size) // For analysis of ratios determined by how many array accesses were required before a new update was made.
{
    double ratio;
    for(int i = 1; i < size; i++)
    {
        ratio = (float)a[i] / (float)b[i];
        cout<<setprecision(2)<<fixed<<ratio<<",  ";
    }
}


int main()
{
    HashArray hasharray;

    string fileLocation;
    ifstream inputFile;

    cout << "\nEnter the location of the text file: ";
    getline(cin, fileLocation);
    inputFile.open(fileLocation.c_str());

    while(inputFile.fail())
    {
        cout << "\nThe file at location " << fileLocation.c_str() << " failed to open.";
        cout << "\nPlease enter a different location:\n";
        getline(cin, fileLocation);
        inputFile.open(fileLocation.c_str());
    }

    string temp;
    while(inputFile >> temp) // Generating Hash Table
		hasharray.update(temp);
	inputFile.close();

    cout << "\nHash table generate successfully.";
    cout << "\nThere are " << hasharray.hashsize() << " unique entries.";

    bool menuON = true;
    char selection;

    while(menuON)
    {
        cout << "\nMenu";
        cout << "\n======================================================";
        cout << "\n    Choose your option:";
        cout << "\nP - Print most frequent words";
        cout << "\nU - Print all unique words";
        cout << "\nR - Generate update ratios";
        cout << "\nE - Export contents to a file";
        cout << "\nX - Exit";
        cout << "\n=======================================================";
        cout << "\n\nEnter your selection: ";
        cin >> selection;

        switch(selection)
        {
            case 'P':
            case 'p':
            {
                int n;
                cout << "\nHow many words do you wish to see? Answer: ";
                cin >> n;
                    while((n < 0) || n > hasharray.hashsize())
                    {
                        cout << "\n Entry out of scope. Please try again: ";
                        cin >> n;
                    }
                cout << "\nPrinting top "<< n <<" most often used words after quicksorting: \n";
                quicksort(hasharray.hasharr);//sorting the hash array by smallest frequency first
            	hasharray.printBackN(n);
            	break;
            }
            case 'U':
            case 'u':
            {
                cout <<"\nPrinting all words appearing only once: \n\n";
                hasharray.find1timewords();
                break;
            }
            case 'R':
            case 'r':
            {
                cout << "\nPrinting all the ratios type cost[i]/update[i] where i = size of array while updating: \n\n";
                getratios(hasharray.cost, hasharray.updates, hasharray.hashsize());
                break;
            }
            case 'E':
            case 'e':
            {
                cout << "\nEnter name of file you wish to export the hash contents to:\n";
                string fileLocation2;
                ofstream exportFile;
                cin.ignore();
                getline(cin, fileLocation2);
                exportFile.open(fileLocation2.c_str());

//                while(exportFile.fail())
//                {
//                    cout << "\nThe file at location " << fileLocation2.c_str() << " failed to open.";
//                    cout << "\nPlease enter a different location: \n";
//                    getline(cin, fileLocation2);
//                    exportFile.open(fileLocation2.c_str());
//                }

                cout <<"\nWriting contents to file in descending order of frequency...\n";
                quicksort(hasharray.hasharr);//sorting the hash array by smallest frequency first
                exportFile << "WORD       FREQUENCY\n----------------------------\n";
                for (int i = HASHSIZE - 1; i >= 0; --i)
                {
                    if(hasharray.hasharr[i].word == "NULL")
                        break;
                    exportFile << hasharray.hasharr[i].word << "        " << hasharray.hasharr[i].frequency << endl;
                }
                exportFile.close();
                break;
            }
            case 'X':
            case 'x':
            {
                menuON = false;
                break;
            }
            default:
            {
                cout << "\nInvalid Selection. Try again or enter 'x' to exit.\n";
            }
        }
    }
    return 0;
}
