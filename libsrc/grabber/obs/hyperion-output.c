#include <util/threading.h>
#include <obs-module.h>

extern void hyperion_receive_video(void *data, struct video_data *frame);
extern void set_image_size(uint32_t width, uint32_t height);

struct hyperion_output
{
	obs_output_t *output;
	pthread_t stop_thread;
	bool stop_thread_active;
};

static const char *hyperion_output_getname(void *unused)
{
	UNUSED_PARAMETER(unused);
	return "Hyperion OBS Output";
}

static void *hyperion_output_create(obs_data_t *settings, obs_output_t *output)
{
	struct hyperion_output *context = bzalloc(sizeof(*context));
	context->output = output;
	UNUSED_PARAMETER(settings);
	return context;
}

static void hyperion_output_destroy(void *data)
{
	struct hyperion_output *context = data;
	if (context->stop_thread_active)
		pthread_join(context->stop_thread, NULL);
	bfree(context);
}

static bool hyperion_output_start(void *data)
{
	struct hyperion_output *context = data;

	if (!obs_output_can_begin_data_capture(context->output, 0))
		return false;

	video_t *video = obs_output_video(context->output);

	set_image_size(video_output_get_width(video), video_output_get_width(video));

	enum video_format format = video_output_get_format(video);

	double video_frame_rate = video_output_get_frame_rate(video);

	if (context->stop_thread_active)
		pthread_join(context->stop_thread, NULL);

	obs_output_begin_data_capture(context->output, 0);
	return true;
}

static void *stop_thread(void *data)
{
	struct hyperion_output *context = data;
	obs_output_end_data_capture(context->output);
	context->stop_thread_active = false;
	return NULL;
}

static void hyperion_output_stop(void *data, uint64_t ts)
{
	struct hyperion_output *context = data;
	UNUSED_PARAMETER(ts);

	context->stop_thread_active = pthread_create(&context->stop_thread,
						     NULL, stop_thread,
						     data) == 0;
}

struct obs_output_info hyperion_output_info =
{
	.id = "hyperion_output",
	.flags = OBS_OUTPUT_VIDEO,
	.get_name = hyperion_output_getname,
	.create = hyperion_output_create,
	.destroy = hyperion_output_destroy,
	.start = hyperion_output_start,
	.stop = hyperion_output_stop,
	.raw_video = hyperion_receive_video
};
