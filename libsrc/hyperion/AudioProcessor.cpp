#include <hyperion/AudioProcessor.h>

#include <utils/Logger.h>
#include <cmath>
#include <vector>
#include <assert.h>

std::vector<double> computeKernel(int n_points, int x_position, std::vector<double> x_values)
{
    //Compute the kernel for the given x point
    std::vector<double> kernel(n_points);
    double sigma  = 0.4;
    double sum_kernel = 0;
    for (int i =0; i<n_points;i++)
    {
        //Compute gaussian kernel
        kernel[i] = exp(-(pow(x_values[i] - x_position,2) / (2*pow(sigma,2))));
        //compute a weight for each kernel position
        sum_kernel += kernel[i];
    }
    //apply weight to each kernel position to give more important value to the x that are around ower x
    for(int i = 0;i<n_points;i++)
        kernel[i] = kernel[i] / sum_kernel;
    return kernel;
}

double applyKernel(int n_points, int x_position, std::vector<double> kernel, std::vector<double> y_values)
{
    std::vector<double> smoothed_vals(n_points);
    double y_filtered = 0;
    //apply filter to all the y values with the weighted kernel
    for(int i = 0;i<n_points;i++)
        y_filtered += kernel[i] * y_values[i];

    //store the filtered value for this x_postions
    smoothed_vals[x_position] = y_filtered;
    return y_filtered;
}

std::vector<double> gauss(const std::vector<double> input)
{
    size_t n_points = input.size();
    std::vector<double> x_values(n_points);
    //Initiate x array with values from 0 -> n_points
    for (int i = 0;i<n_points;i++)
        x_values[i] = i;

    std::vector<double> y_values_filtered(n_points);

    //apply filter to every points
    for (int x_position=0;x_position<n_points;x_position++)
    {
        std::vector<double> kernel(n_points);
        kernel = computeKernel(n_points,x_position,x_values);
        double y_filtered = applyKernel(n_points, x_position,kernel, input);
        y_values_filtered[x_position] = y_filtered;
    }
	return y_values_filtered;
}

AudioProcessor::AudioProcessor(const LedString& ledString, Hyperion* hyperion) :
	  _log(Logger::getInstance("AUDIO"))
	, _ledString()
	, _hyperion(hyperion)
	, _input(nullptr)
	, _output(nullptr)
	, _plan()
{
	int input_size = 1024;
	int output_size = (input_size / 2 + 1);

	_input = static_cast<double*>(fftw_malloc(input_size * sizeof(double)));
	_output = static_cast<fftw_complex*>(fftw_malloc(output_size * sizeof(fftw_complex)));

	int flags = FFTW_ESTIMATE;
	_plan = fftw_plan_dft_r2c_1d(input_size, _input, _output, flags);

	/* TODO : MURSE */
	dbexpsmth.set_1st_smoothing_constant(0.001);
	dbexpsmth.set_2nd_smoothing_constant(0.99);
	//dbexpsmth.set_vacillation_tolerance(0.1);
}

AudioProcessor::~AudioProcessor()
{
	fftw_free(_input);
	fftw_free(_output);
	fftw_destroy_plan(_plan);
}

std::string v_d_to_string(v_d vec)
{
	if (vec.empty())
		return "empty";

	std::stringstream stream;
	for (int i=0; i < vec.size() - 1; i++)
	{
		stream << std::scientific << vec[i];
		stream << ", ";
	}
	stream << std::scientific << vec.back();
	stream << "\n";
	return stream.str();
}

#define DIMENSION 13

std::vector<ColorRgb> AudioProcessor::process(const AudioPacket& packet)
{
	std::vector<ColorRgb> colors;

	v_d out;
	auto src = packet.memptr();
	size_t size = packet.size();
	size_t window_size = 160;

	_buffer.insert(_buffer.end(), src, src + size);
	if (_buffer.size() < window_size)
		return _prevColors;

	src = _buffer.data();
	size = _buffer.size();
	while (size >= window_size)
	{
		out   = _mfcc.processFrame(src, window_size);
		size -= window_size;
		src  += window_size;
	}

	_buffer.erase(_buffer.begin(), _buffer.end() - size);

	if (out.empty())
		return _prevColors;

	Debug(_log, "ORIGINAL Y : %s",  v_d_to_string(out).c_str());

        es_vec<double, DIMENSION> curr_query;

        for (size_t i = 0; i < DIMENSION; ++i)
        	curr_query[i] = std::abs(out[i]);


        // smoothing
        es_vec<double, DIMENSION> smth_result = dbexpsmth.push_to_pop(curr_query);

	v_d outPrinter;
	outPrinter.assign(smth_result.raw_data(), smth_result.raw_data() + DIMENSION);
	Debug(_log, "GAIN : %s",  v_d_to_string(outPrinter).c_str());

        for (size_t i = 0; i < DIMENSION; ++i)
        	out[i] /= smth_result[i];


	Debug(_log, "GAINED Y: %s",  v_d_to_string(out).c_str());


        for (size_t i = 0; i < DIMENSION; ++i)
        	out[i] *= 29;


	Debug(_log, "SCALED Y: %s",  v_d_to_string(out).c_str());

	double r = 0.0;
	double g = 0.0;
	double b = 0.0;

	const double scale = 0.9;

	/* We have 28 features. We skip the
	 * first two as they are constant */
	for (int i = 1; i < 5; ++i)
	{
		r += std::pow(std::abs(out[i]    ), scale);
		g += std::pow(std::abs(out[i + 4]), scale);
		b += std::pow(std::abs(out[i + 8]), scale);
	}

	//r = r / 4;
	//g = g / 4;
	//b = b / 4;

	Info(_log, "r: %f g %f b %f", r, g, b);

	std::vector<double> r_vec, g_vec, b_vec;
	for (int i = 0; i < 120; ++i)
	{
		r_vec.push_back(r > i ? 255 : 0);
		g_vec.push_back(g > i ? 255 : 0);
		b_vec.push_back(b > i ? 255 : 0);
	}

	r_vec = gauss(r_vec);
	g_vec = gauss(g_vec);
	b_vec = gauss(b_vec);

	for (int i = 0; i < 120; ++i)
	{
		colors.push_back({
			r_vec[i],
			g_vec[i],
			b_vec[i]});
	}

	// fftw_execute(_plan);

	_prevColors = colors;

	return colors;
}
