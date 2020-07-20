#pragma once

#include <QSharedDataPointer>

#include <utils/AudioPacketData.h>

class AudioPacket
{
public:
	AudioPacket() :
		AudioPacket(0)
	{
	}

	AudioPacket(size_t size) :
		_d_ptr(new AudioPacketData(size))
	{
	}

	AudioPacket(const AudioPacket & other)
	{
		_d_ptr = other._d_ptr;
	}

	AudioPacket& operator=(AudioPacket rhs)
	{
		_d_ptr = rhs._d_ptr;
		return *this;
	}

	void swap(AudioPacket& s)
	{
		std::swap(this->_d_ptr, s._d_ptr);
	}

	AudioPacket(AudioPacket&& src) noexcept
	{
		std::swap(this->_d_ptr, src._d_ptr);
	}

	AudioPacket& operator=(AudioPacket&& src) noexcept
	{
		src.swap(*this);
		return *this;
	}
	~AudioPacket()
	{
	}

	bool empty()const
	{
		return _d_ptr->empty();
	}

	void resize(size_t size)
	{
		_d_ptr->resize(size);
	}

	int16_t* memptr()
	{
		return _d_ptr->memptr();
	}

	const int16_t* memptr() const
	{
		return _d_ptr->memptr();
	}

	size_t size() const
	{
		return _d_ptr->size();
	}

	void clear()
	{
		_d_ptr->clear();
	}

private:
	QSharedDataPointer<AudioPacketData>  _d_ptr;
};
