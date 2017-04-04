template <class T>
inline gfx::Buffer<T>::Buffer(
	VertexArray *vao,
	GLenum target,
	GLenum usage,
	unsigned int itemSize,
	unsigned int numItems) :
	VAO(vao),
	Target(target),
	Usage(usage),
	ItemSize(itemSize),
	NumItems(numItems),
	Data(NULL),
	managed_data(false)
{
	glGenBuffers(1, &Name);
}

template <class T>
inline gfx::Buffer<T>::~Buffer()
{
	glDeleteBuffers(1, &Name);
	if (managed_data)
		delete[] Data;
}

template <class T>
inline void gfx::Buffer<T>::Reinit(
	VertexArray *vao, GLenum target, GLenum usage, unsigned int itemSize,
	unsigned int numItems)
{
	glDeleteBuffers(1, &Name);
	if (managed_data)
		delete[] Data;
	VAO = vao;
	Target = target;
	Usage = usage;
	ItemSize = itemSize;
	NumItems = numItems;
	Data = NULL;
	managed_data = false;
	glGenBuffers(1, &Name);
}

template <class T>
inline void gfx::Buffer<T>::SetData(T *data, bool copy)
{
	if (managed_data)
	{
		managed_data = false;
		delete[] Data;
		Data = NULL;
	}
	if (copy && data != NULL)
	{
		Data = new T[NumItems * ItemSize];
		memcpy(Data, data, sizeof(T) * ItemSize * NumItems);
		managed_data = true;
	}
	VAO->SetBufferData(
		Name,
		Target,
		sizeof(T) * ItemSize * NumItems,
		Data,
		Usage);
}

template <class T>
inline void gfx::Buffer<T>::BindBuffer()
{
	VAO->Bind();
	glBindBuffer(Target, Name);
	VAO->Unbind();
}