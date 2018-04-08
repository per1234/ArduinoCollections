/**
 * A Dictionary that has fixed size and uses chars as keys.
 **/
#ifndef CharDictionary_HEADER
#define CharDictionary_HEADER

#if defined CharDictionary_TEST
	#include <cstring>
	typedef unsigned char byte;
#endif

template <typename T_VAL>
class FixedCharDictionary {
	private:
		void populate(const byte entryc, char keys[], T_VAL values[]) {
			char tmpKeys[entryc] = {};
			T_VAL tmpVals[entryc] = {};
			byte tmpSize = 0;

			for(int i=0; i<entryc; i++) {
				short l = 0;
				short r = tmpSize-1;
				short m = l + (r-l)/2;

				// Search the position where to put the entry
				while(l <= r) {
					m = l + (r-l)/2;
					if(keys[i] > tmpKeys[m]){
						l = m+1;
					} else if(keys[i] < tmpKeys[m]) {
						r = m-1;
					} else if(keys[i] == tmpKeys[m]) {
						break;
					}
				}

				// Insert the entry
				if(keys[i] == tmpKeys[m]) {
					// Replace the old value
					tmpVals[m] = values[i];
				} else {
					memmove(tmpKeys+l+1, tmpKeys+l, (tmpSize-l) * sizeof(char));
					memmove(tmpVals+l+1, tmpVals+l, (tmpSize-l) * sizeof(T_VAL));
					tmpKeys[l] = keys[i];
					tmpVals[l] = values[i];
					tmpSize++;
				}
			}

			mContainer.size = tmpSize;
			mKeys = new char[mContainer.size];
			mValues = new T_VAL[mContainer.size];

			memcpy(mKeys, tmpKeys, tmpSize * sizeof(char));
			memcpy(mValues, tmpVals, tmpSize * sizeof(T_VAL));
		}
		struct S {
			byte size:7; // Size can only be from 0 to 128
			byte lastEntryIdx:7; // The Index of the last found Entry. Used to optmise "hasKey() -> get()" sequences.
			// TODO use remaining 2 Bits
		} mContainer;
		char* mKeys; // Array containing the keys of all Dictionary entries.
		T_VAL* mValues; //  Array containing the values of all Dictionary entries.
	public:
		explicit FixedCharDictionary(const byte entryc, char keys[], T_VAL values[]) : mContainer({0,0}) {
			populate(entryc, keys, values);
		}

		/**
		 * Returns the number of Dictionary entries.
		 **/
		byte size() {
			return mContainer.size;
		}

		/**
		 * Returns true if the Dictionary contains an entry with that key, false otherwise.
		 **/
		bool hasKey(const char key) {
			if(mKeys[mContainer.lastEntryIdx] == key) return true;

			short l = 0;
			short r = mContainer.size-1;
			short m = l + (r-l)/2;

			// Search for the key in mKeys using binary search.
			while(l <= r) {
				m = l + (r-l)/2;
				if(key > mKeys[m]){
					l = m+1;
				} else if(key < mKeys[m]) {
					r = m-1;
				} else if(key == mKeys[m]) {
					// Found key. Return true
					mContainer.lastEntryIdx = m;
					return true;
				}
			}

			// Key wasn't found. Return false.
			return false;
		}

		/**
		 * Returns the value of the Dictionary entry with that key or null if it doesn't exist.
		 **/
		T_VAL* get(const char key) {
			if(hasKey(key)) return &mValues[mContainer.lastEntryIdx];
			else return NULL;
		};

		/**
		 * Updates the value of an Entry. Returns false if an Entry with that key doesn't exist.
		 **/
		bool update(const char key, T_VAL value) {
			if(hasKey(key)) {
				mValues[mContainer.lastEntryIdx] = value;
				return true;
			} else return false;
		}

		char* keys() {
			return mKeys;
		}

		T_VAL* values() {
			return mValues;
		}

		/**
		 * Deconstructor
		 **/
		~FixedCharDictionary() {
			delete[] mKeys;
			delete[] mValues;
		}
};

#endif