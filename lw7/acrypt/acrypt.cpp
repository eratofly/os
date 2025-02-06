#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <aio.h>
#include <cstring>
#include <vector>
#include <stdexcept>

const int64_t BUFFER_SIZE = 1024 * 1024 * 1024; // 1 GB

unsigned char invertBits(unsigned char byte)
{
    return ~byte;
}

class AIOFileHandler
{
public:
    AIOFileHandler(const char *inputFilename, const char *outputFilename)
            : inputFd(open(inputFilename, O_RDONLY)),
              outputFd(open(outputFilename, O_WRONLY | O_CREAT | O_TRUNC, 0644)),
              readBuffer(BUFFER_SIZE),
              writeBuffer(BUFFER_SIZE),
              writeBufferSize(0)
    {
        if (inputFd == -1)
        {
            throw std::runtime_error("Error opening input file");
        }

        if (outputFd == -1)
        {
            close(inputFd);
            throw std::runtime_error("Error opening output file");
        }

        memset(&readIo, 0, sizeof(struct aiocb));
        memset(&writeIo, 0, sizeof(struct aiocb));

        readIo.aio_fildes = inputFd;
        readIo.aio_buf = readBuffer.data();
        readIo.aio_nbytes = BUFFER_SIZE;
        readIo.aio_offset = 0;

        writeIo.aio_fildes = outputFd;
        writeIo.aio_buf = writeBuffer.data();
        writeIo.aio_nbytes = 0;
        writeIo.aio_offset = 0;
    }

    ~AIOFileHandler()
    {
        if (inputFd != -1)
        {
            close(inputFd);
        }
        if (outputFd != -1)
        {
            close(outputFd);
        }
    }

    void ProcessAsyncIO()
    {
        if (aio_read(&readIo) == -1)
        {
            std::cerr << "Error initiating read: " << strerror(errno) << std::endl;
            return;
        }

        while (true)
        {
            while (aio_error(&readIo) == EINPROGRESS)
            {
                continue;
            }

            int readStatus = aio_error(&readIo);
            if (readStatus != 0)
            {
                if (readStatus != ECANCELED)
                {
                    std::cerr << "Error during read: " << strerror(errno) << std::endl;
                }
                break;
            }

            ssize_t bytesRead = aio_return(&readIo);
            if (bytesRead <= 0)
            {
                WriteAndClearWriteBuffer();
                break;
            }

            for (ssize_t i = 0; i < bytesRead; ++i)
            {
                writeBuffer[writeBufferSize++] = invertBits(readBuffer[i]);

                if (writeBufferSize == BUFFER_SIZE)
                {
                    WriteAndClearWriteBuffer();
                }
            }

            readIo.aio_offset += bytesRead;
            if (aio_read(&readIo) == -1)
            {
                std::cerr << "Error initiating read: " << strerror(errno) << std::endl;
                break;
            }
        }
    }

private:
    int inputFd = -1;
    int outputFd = -1;
    std::vector<unsigned char> readBuffer;
    std::vector<unsigned char> writeBuffer;
    size_t writeBufferSize;
    struct aiocb readIo{};
    struct aiocb writeIo{};

    void WriteAndClearWriteBuffer()
    {
        if (writeBufferSize > 0)
        {
            writeIo.aio_nbytes = writeBufferSize;
            if (aio_write(&writeIo) == -1)
            {
                std::cerr << "Error initiating write: " << strerror(errno) << std::endl;
                return;
            }

            while (aio_error(&writeIo) == EINPROGRESS)
            {
                continue;
            }

            int writeStatus = aio_error(&writeIo);
            if (writeStatus != 0)
            {
                std::cerr << "Error during write: " << strerror(errno) << std::endl;
                return;
            }

            ssize_t bytesWritten = aio_return(&writeIo);
            if (bytesWritten != writeBufferSize)
            {
                std::cerr << "Incomplete write occurred!" << std::endl;
                return;
            }

            writeIo.aio_offset += bytesWritten;
            writeBufferSize = 0; // очищаю буфер
        }
    }
};

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: acrypt INPUT_FILE_NAME OUTPUT_FILE_NAME" << std::endl;
        return 1;
    }

    try
    {
        AIOFileHandler fileHandler(argv[1], argv[2]);
        fileHandler.ProcessAsyncIO();
    } catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}