#pragma once

#include <QSharedData>

class AudioPacketData : public QSharedData
{
public:
	AudioPacketData(size_t size) :
            _size(size),
            _data(nullptr)
	{
		if (_size)
		{
			_data = new int16_t[size];
			memset(_data, 0, _size);
		}
	}

	AudioPacketData(const AudioPacketData & other) :
		QSharedData(other),
		_size(other._size),
		_data(other._data)
	{
		if (_size)
		{
			_data = new int16_t[_size];
			memcpy(_data, other._data, other._size);
		}
	}

	AudioPacketData& operator=(AudioPacketData rhs)
	{
		rhs.swap(*this);
		return *this;
	}

	void swap(AudioPacketData& s) noexcept
	{
		using std::swap;
		swap(this->_size, s._size);
		swap(this->_data, s._data);
	}

	AudioPacketData(AudioPacketData&& src) noexcept
		: _size(0)
		, _data(nullptr)
	{
		src.swap(*this);
	}

	AudioPacketData& operator=(AudioPacketData&& src) noexcept
	{
		src.swap(*this);
		return *this;
	}

	~AudioPacketData()
	{
		delete[] _data;
	}

	bool empty() const
	{
	    return _size == 0;
	}

	void resize(size_t size)
	{
		if (size == _size)
			return;

		if (size > _size)
		{
			delete[] _data;
			_data = new int16_t[size];
		}

		_size = size;
	}

	int16_t* memptr()
	{
		return _data;
	}

	const int16_t* memptr() const
	{
		return _data;
	}

	size_t size() const
	{
		return  _size;
	}

	void clear()
	{
		_size = 0;
		delete[] _data;
		_data = nullptr;
	}

private:
	size_t _size;
	int16_t * _data;
};
