//http://blogs.msdn.com/b/nativeconcurrency/archive/2012/01/17/c-amp-it-s-got-character-but-no-char.aspx

// Read character at index idx from array arr.
template <typename T>
unsigned int read_uchar(T& arr, int idx) restrict(amp)
{
    return (arr[idx >> 2] & (0xFF << ((idx & 0x3) << 3))) >> ((idx & 0x3) << 3);
}

// Increment character at index idx in array arr.
template<typename T>
void increment_uchar(T& arr, int idx) restrict(amp)
{
    atomic_fetch_add(&arr[idx >> 2], 1 << ((idx & 0x3) << 3));
}

// Add value val to character at index idx in array arr.
template<typename T>
void addto_uchar(T& arr, int idx, unsigned int val) restrict(amp)
{
    atomic_fetch_add(&arr[idx >> 2], (val & 0xFF) << ((idx & 0x3) << 3));
}

// Write value val to character at index idx in array arr.
template<typename T>
void write_uchar(T& arr, int idx, unsigned int val) restrict(amp)
{
    atomic_fetch_xor(&arr[idx >> 2], arr[idx >> 2] & (0xFF << ((idx & 0x3) << 3)));
    atomic_fetch_xor(&arr[idx >> 2], (val & 0xFF) << ((idx & 0x3) << 3));
}

// Helper function to accept 1D indices of index<1> type instead of integers.
template <typename T>
unsigned int read_uchar(T& arr, index<1> idx) restrict(amp) { return read_uchar(arr, idx[0]); }
template<typename T>
void increment_uchar(T& arr, index<1> idx) restrict(amp) { increment_uchar(arr, idx[0]); }
template<typename T>
void addto_uchar(T& arr, index<1> idx, unsigned int val) restrict(amp) { addto_uchar(arr, idx[0], val); }
template<typename T>
void write_uchar(T& arr, index<1> idx, unsigned int val) restrict(amp) { write_uchar(arr, idx[0], val); }