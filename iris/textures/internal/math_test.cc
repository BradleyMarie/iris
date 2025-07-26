#include "iris/textures/internal/math.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/point.h"

namespace iris {
namespace textures {
namespace internal {
namespace {

TEST(Math, PerlinNoise) {
  EXPECT_NEAR(-0.0646866, PerlinNoise(Point(-255.351, -88.4691, 108.674)),
              0.01);
  EXPECT_NEAR(-0.0603888, PerlinNoise(Point(-253.194, 40.7393, 150.243)), 0.01);
  EXPECT_NEAR(0.314079, PerlinNoise(Point(-252.441, -39.4668, -164.509)), 0.01);
  EXPECT_NEAR(0.265385, PerlinNoise(Point(-251.82, -117.839, -22.1481)), 0.01);
  EXPECT_NEAR(0.289888, PerlinNoise(Point(-250.614, 80.0508, 54.3055)), 0.01);
  EXPECT_NEAR(-0.397744, PerlinNoise(Point(-247.513, -221.048, -155.425)),
              0.01);
  EXPECT_NEAR(0.55063, PerlinNoise(Point(-247.344, 62.3183, -195.927)), 0.01);
  EXPECT_NEAR(0.122581, PerlinNoise(Point(-243.007, 73.6952, -111.881)), 0.01);
  EXPECT_NEAR(0.195398, PerlinNoise(Point(-243.31, 170.432, -192.792)), 0.01);
  EXPECT_NEAR(-0.000495598, PerlinNoise(Point(-239.89, 51.8486, -127.751)),
              0.01);
  EXPECT_NEAR(0.162205, PerlinNoise(Point(-239.906, 54.2015, -232.549)), 0.01);
  EXPECT_NEAR(0.102792, PerlinNoise(Point(-239.818, 154.703, -71.7438)), 0.01);
  EXPECT_NEAR(-0.15613, PerlinNoise(Point(-239.625, 177.002, -165.023)), 0.01);
  EXPECT_NEAR(0.160647, PerlinNoise(Point(-239.263, 211.576, 140.01)), 0.01);
  EXPECT_NEAR(-0.530604, PerlinNoise(Point(-238.763, -132.669, -32.9664)),
              0.01);
  EXPECT_NEAR(-0.548091, PerlinNoise(Point(-227.297, 85.1498, -170.66)), 0.01);
  EXPECT_NEAR(-0.508425, PerlinNoise(Point(-223.946, 219.717, -11.7119)), 0.01);
  EXPECT_NEAR(-0.223923, PerlinNoise(Point(-222.728, -200.254, -24.7189)),
              0.01);
  EXPECT_NEAR(-0.0215767, PerlinNoise(Point(-222.589, 1.07061, 116.569)), 0.01);
  EXPECT_NEAR(-0.419815, PerlinNoise(Point(-222.229, 231.493, 21.8915)), 0.01);
  EXPECT_NEAR(-0.231913, PerlinNoise(Point(-221.481, 215.826, -43.8051)), 0.01);
  EXPECT_NEAR(0.246462, PerlinNoise(Point(-221.342, 250.349, 235.971)), 0.01);
  EXPECT_NEAR(-0.311201, PerlinNoise(Point(-220.032, -88.3227, 33.4884)), 0.01);
  EXPECT_NEAR(-0.357453, PerlinNoise(Point(-219.606, 180.17, -250.901)), 0.01);
  EXPECT_NEAR(-0.357313, PerlinNoise(Point(-218.086, -108.756, 145.736)), 0.01);
  EXPECT_NEAR(0.313645, PerlinNoise(Point(-217.427, -201.918, 244.358)), 0.01);
  EXPECT_NEAR(-0.515656, PerlinNoise(Point(-217.207, -109.208, -148.628)),
              0.01);
  EXPECT_NEAR(0.14165, PerlinNoise(Point(-217.378, -99.192, -47.2646)), 0.01);
  EXPECT_NEAR(-0.569955, PerlinNoise(Point(-216.459, 91.6309, 148.269)), 0.01);
  EXPECT_NEAR(0.579764, PerlinNoise(Point(-214.97, 33.4727, -219.248)), 0.01);
  EXPECT_NEAR(-0.204373, PerlinNoise(Point(-213.078, -81.0707, 172.26)), 0.01);
  EXPECT_NEAR(-0.00892821, PerlinNoise(Point(-213.551, 153.036, 122.965)),
              0.01);
  EXPECT_NEAR(0.0710918, PerlinNoise(Point(-212.644, -251.719, -128.807)),
              0.01);
  EXPECT_NEAR(0.482299, PerlinNoise(Point(-212.551, 51.2907, 213.472)), 0.01);
  EXPECT_NEAR(-0.346024, PerlinNoise(Point(-211.85, 213.183, -32.3868)), 0.01);
  EXPECT_NEAR(0.135457, PerlinNoise(Point(-210.506, 219.454, 30.0858)), 0.01);
  EXPECT_NEAR(0.314787, PerlinNoise(Point(-209.815, -177.177, -162.674)), 0.01);
  EXPECT_NEAR(0.307088, PerlinNoise(Point(-209.836, 233.336, 129.156)), 0.01);
  EXPECT_NEAR(0.0310657, PerlinNoise(Point(-208.419, 75.424, 14.3873)), 0.01);
  EXPECT_NEAR(-0.0124636, PerlinNoise(Point(-204.954, -190.186, 143.209)),
              0.01);
  EXPECT_NEAR(-0.0749623, PerlinNoise(Point(-204.562, 91.0253, 216.505)), 0.01);
  EXPECT_NEAR(0.0871693, PerlinNoise(Point(-201.478, -97.0947, 122.879)), 0.01);
  EXPECT_NEAR(0.304139, PerlinNoise(Point(-198.634, -218.15, -188.005)), 0.01);
  EXPECT_NEAR(0.548268, PerlinNoise(Point(-196.946, -205.465, -122.028)), 0.01);
  EXPECT_NEAR(-0.207742, PerlinNoise(Point(-196.751, -64.1686, 239.866)), 0.01);
  EXPECT_NEAR(0.19676, PerlinNoise(Point(-192.575, -154.713, 226.468)), 0.01);
  EXPECT_NEAR(0.569184, PerlinNoise(Point(-191.419, -221.49, -226.972)), 0.01);
  EXPECT_NEAR(-0.23039, PerlinNoise(Point(-190.775, 141.063, 138.258)), 0.01);
  EXPECT_NEAR(0.69385, PerlinNoise(Point(-189.555, -201.684, -150.106)), 0.01);
  EXPECT_NEAR(-0.0831731, PerlinNoise(Point(-189.251, -75.5169, -178.895)),
              0.01);
  EXPECT_NEAR(-0.371087, PerlinNoise(Point(-188.221, 0.417439, -31.4465)),
              0.01);
  EXPECT_NEAR(0.105357, PerlinNoise(Point(-186.982, -157.784, 226.337)), 0.01);
  EXPECT_NEAR(-0.125221, PerlinNoise(Point(-185.376, 138.617, 9.87456)), 0.01);
  EXPECT_NEAR(0.140765, PerlinNoise(Point(-183.242, -147.346, -64.817)), 0.01);
  EXPECT_NEAR(-0.0851063, PerlinNoise(Point(-183.471, 12.9337, 176.812)), 0.01);
  EXPECT_NEAR(0.0317164, PerlinNoise(Point(-182.081, -98.4915, -114.149)),
              0.01);
  EXPECT_NEAR(-0.446962, PerlinNoise(Point(-181.014, -210.391, 33.9996)), 0.01);
  EXPECT_NEAR(0.355979, PerlinNoise(Point(-181.565, -171.963, 200.684)), 0.01);
  EXPECT_NEAR(-0.0101054, PerlinNoise(Point(-181.98, -160.384, -125.571)),
              0.01);
  EXPECT_NEAR(-0.137614, PerlinNoise(Point(-181.908, -156.189, 144.007)), 0.01);
  EXPECT_NEAR(-0.0265943, PerlinNoise(Point(-179.798, 229.701, -219.143)),
              0.01);
  EXPECT_NEAR(-0.245128, PerlinNoise(Point(-177.654, -100.247, -113.202)),
              0.01);
  EXPECT_NEAR(0.113441, PerlinNoise(Point(-175.298, -21.4728, -17.1406)), 0.01);
  EXPECT_NEAR(-0.157752, PerlinNoise(Point(-174.218, 44.0765, -198.694)), 0.01);
  EXPECT_NEAR(-0.041916, PerlinNoise(Point(-173.83, 212.644, -29.6195)), 0.01);
  EXPECT_NEAR(0.568846, PerlinNoise(Point(-171.008, -226.572, 197.548)), 0.01);
  EXPECT_NEAR(-0.110929, PerlinNoise(Point(-170.039, -243.076, 248.98)), 0.01);
  EXPECT_NEAR(0.0858238, PerlinNoise(Point(-170.141, -115.117, 153.722)), 0.01);
  EXPECT_NEAR(0.0573101, PerlinNoise(Point(-170.654, 131.006, -174.896)), 0.01);
  EXPECT_NEAR(0.0953234, PerlinNoise(Point(-167.375, 55.1404, 149.019)), 0.01);
  EXPECT_NEAR(0.139313, PerlinNoise(Point(-166.296, 6.99663, 7.97418)), 0.01);
  EXPECT_NEAR(0.0692738, PerlinNoise(Point(-166.41, 243.424, -46.5895)), 0.01);
  EXPECT_NEAR(-0.169261, PerlinNoise(Point(-165.69, -204.61, -81.6403)), 0.01);
  EXPECT_NEAR(-0.208049, PerlinNoise(Point(-164.139, -191.689, 216.388)), 0.01);
  EXPECT_NEAR(0.312808, PerlinNoise(Point(-164.779, 9.84977, 159.8)), 0.01);
  EXPECT_NEAR(-0.102097, PerlinNoise(Point(-161.316, -105.985, -26.9281)),
              0.01);
  EXPECT_NEAR(-0.408479, PerlinNoise(Point(-161.823, -83.308, -204.627)), 0.01);
  EXPECT_NEAR(-0.559863, PerlinNoise(Point(-160.194, -199.657, -214.869)),
              0.01);
  EXPECT_NEAR(-0.156655, PerlinNoise(Point(-160.126, 232.44, 197.139)), 0.01);
  EXPECT_NEAR(0.197274, PerlinNoise(Point(-158.482, 143.575, 52.7807)), 0.01);
  EXPECT_NEAR(0.0818087, PerlinNoise(Point(-156.036, -105.049, -193.682)),
              0.01);
  EXPECT_NEAR(0.0160218, PerlinNoise(Point(-156.682, 226.646, -79.569)), 0.01);
  EXPECT_NEAR(-0.089234, PerlinNoise(Point(-155.465, -253.615, -204.325)),
              0.01);
  EXPECT_NEAR(-0.316718, PerlinNoise(Point(-155.023, 26.8689, -73.402)), 0.01);
  EXPECT_NEAR(0.561655, PerlinNoise(Point(-154.208, 10.6263, -186.622)), 0.01);
  EXPECT_NEAR(-0.0232592, PerlinNoise(Point(-154.474, 146.468, 6.59766)), 0.01);
  EXPECT_NEAR(-0.187922, PerlinNoise(Point(-153.266, -143.554, 28.75)), 0.01);
  EXPECT_NEAR(0.0108727, PerlinNoise(Point(-153.091, -88.3099, -23.6405)),
              0.01);
  EXPECT_NEAR(0.0795933, PerlinNoise(Point(-153.049, -18.8829, -194.21)), 0.01);
  EXPECT_NEAR(-0.334919, PerlinNoise(Point(-152.889, -3.2614, 118.021)), 0.01);
  EXPECT_NEAR(-0.433029, PerlinNoise(Point(-151.426, -182.793, 89.8694)), 0.01);
  EXPECT_NEAR(-0.411071, PerlinNoise(Point(-149.295, 115.528, 178.03)), 0.01);
  EXPECT_NEAR(-0.505891, PerlinNoise(Point(-147.521, 15.9268, -106.515)), 0.01);
  EXPECT_NEAR(-0.276969, PerlinNoise(Point(-146.461, -74.7768, -40.605)), 0.01);
  EXPECT_NEAR(-0.506832, PerlinNoise(Point(-145.497, -242.122, -68.6295)),
              0.01);
  EXPECT_NEAR(0.180617, PerlinNoise(Point(-145.959, 131.576, -246.46)), 0.01);
  EXPECT_NEAR(-0.194557, PerlinNoise(Point(-144.818, 114.881, 199.062)), 0.01);
  EXPECT_NEAR(-0.406754, PerlinNoise(Point(-143.494, -194.752, 162.947)), 0.01);
  EXPECT_NEAR(-0.484581, PerlinNoise(Point(-143.051, -189.403, -238.533)),
              0.01);
  EXPECT_NEAR(0.345168, PerlinNoise(Point(-142.045, 80.7824, 88.4211)), 0.01);
  EXPECT_NEAR(0.0520379, PerlinNoise(Point(-141.021, -171.006, -37.0463)),
              0.01);
  EXPECT_NEAR(0.0416534, PerlinNoise(Point(-140.632, -54.7546, 110.122)), 0.01);
  EXPECT_NEAR(-0.153425, PerlinNoise(Point(-139.606, 40.3334, 88.4517)), 0.01);
  EXPECT_NEAR(-0.20701, PerlinNoise(Point(-137.005, -30.0803, -150.248)), 0.01);
  EXPECT_NEAR(-0.195952, PerlinNoise(Point(-137.269, 64.9318, 226.672)), 0.01);
  EXPECT_NEAR(0.185667, PerlinNoise(Point(-136.358, -102.632, -140.183)), 0.01);
  EXPECT_NEAR(-0.0473518, PerlinNoise(Point(-135.781, -206.848, 85.4162)),
              0.01);
  EXPECT_NEAR(0.323851, PerlinNoise(Point(-132.311, 126.343, -158.186)), 0.01);
  EXPECT_NEAR(0.504999, PerlinNoise(Point(-131.732, -40.7821, -216.179)), 0.01);
  EXPECT_NEAR(0.404007, PerlinNoise(Point(-130.772, 17.3995, -246.13)), 0.01);
  EXPECT_NEAR(-0.22358, PerlinNoise(Point(-124.371, 120.731, 130.691)), 0.01);
  EXPECT_NEAR(-0.606782, PerlinNoise(Point(-124.664, 201.899, -92.2942)), 0.01);
  EXPECT_NEAR(-0.0157118, PerlinNoise(Point(-122.324, 100.345, -46.4763)),
              0.01);
  EXPECT_NEAR(0.044443, PerlinNoise(Point(-121.418, -9.79119, 210.26)), 0.01);
  EXPECT_NEAR(0.668184, PerlinNoise(Point(-121.59, 200.38, 203.701)), 0.01);
  EXPECT_NEAR(-0.0259155, PerlinNoise(Point(-120.614, 126.666, 237.089)), 0.01);
  EXPECT_NEAR(0.0476894, PerlinNoise(Point(-119.079, -113.008, -162.145)),
              0.01);
  EXPECT_NEAR(-0.332652, PerlinNoise(Point(-119.565, 14.6864, 231.507)), 0.01);
  EXPECT_NEAR(-0.420643, PerlinNoise(Point(-116.154, 2.2951, 27.8903)), 0.01);
  EXPECT_NEAR(0.205546, PerlinNoise(Point(-114.347, -194.467, -68.2501)), 0.01);
  EXPECT_NEAR(-0.26301, PerlinNoise(Point(-112.364, -126.343, 156.705)), 0.01);
  EXPECT_NEAR(0.00191577, PerlinNoise(Point(-112.79, 132.09, -1.64901)), 0.01);
  EXPECT_NEAR(-0.0259886, PerlinNoise(Point(-111.902, -181.415, -5.33304)),
              0.01);
  EXPECT_NEAR(-0.0307877, PerlinNoise(Point(-111.362, 104.162, -227.873)),
              0.01);
  EXPECT_NEAR(-0.0927288, PerlinNoise(Point(-108.236, 125.039, -28.5535)),
              0.01);
  EXPECT_NEAR(0.0340037, PerlinNoise(Point(-107.283, -136.662, 77.1601)), 0.01);
  EXPECT_NEAR(-0.00092572, PerlinNoise(Point(-107.238, 171.303, 79.3708)),
              0.01);
  EXPECT_NEAR(-0.0398989, PerlinNoise(Point(-106.929, -236.133, -84.7893)),
              0.01);
  EXPECT_NEAR(-0.147124, PerlinNoise(Point(-106.398, 214.995, 248.845)), 0.01);
  EXPECT_NEAR(-0.19243, PerlinNoise(Point(-104.647, -55.0911, 133.005)), 0.01);
  EXPECT_NEAR(0.178138, PerlinNoise(Point(-100.528, -73.1608, 211.944)), 0.01);
  EXPECT_NEAR(0.758966, PerlinNoise(Point(-97.7441, 48.6006, -180.302)), 0.01);
  EXPECT_NEAR(-0.0919813, PerlinNoise(Point(-97.2297, 126.804, 10.9908)), 0.01);
  EXPECT_NEAR(-0.185162, PerlinNoise(Point(-97.2242, 233.134, 237.543)), 0.01);
  EXPECT_NEAR(-0.152526, PerlinNoise(Point(-94.0355, -231.171, 73.7717)), 0.01);
  EXPECT_NEAR(0.24788, PerlinNoise(Point(-93.1112, -50.8751, -116.841)), 0.01);
  EXPECT_NEAR(-0.18552, PerlinNoise(Point(-91.3261, -255.017, 22.6465)), 0.01);
  EXPECT_NEAR(0.148506, PerlinNoise(Point(-90.5948, -169.385, -207.761)), 0.01);
  EXPECT_NEAR(0.353752, PerlinNoise(Point(-90.7431, 243.411, 38.135)), 0.01);
  EXPECT_NEAR(-0.0539811, PerlinNoise(Point(-89.6483, -45.1078, -2.39786)),
              0.01);
  EXPECT_NEAR(-0.314216, PerlinNoise(Point(-88.4296, -148.195, 255.167)), 0.01);
  EXPECT_NEAR(-0.443237, PerlinNoise(Point(-87.9509, -75.4514, -65.0482)),
              0.01);
  EXPECT_NEAR(-0.254457, PerlinNoise(Point(-86.1429, 92.0606, -178.749)), 0.01);
  EXPECT_NEAR(0.194342, PerlinNoise(Point(-85.8926, -108.926, -64.2285)), 0.01);
  EXPECT_NEAR(-0.0916786, PerlinNoise(Point(-85.6811, -84.4575, 100.35)), 0.01);
  EXPECT_NEAR(0.272779, PerlinNoise(Point(-85.5208, 98.751, 54.3499)), 0.01);
  EXPECT_NEAR(-0.270104, PerlinNoise(Point(-85.0072, 167.504, 85.4218)), 0.01);
  EXPECT_NEAR(-0.185695, PerlinNoise(Point(-84.1752, -221.423, 140.7)), 0.01);
  EXPECT_NEAR(-0.0826099, PerlinNoise(Point(-84.8539, -142.581, -112.012)),
              0.01);
  EXPECT_NEAR(0.46053, PerlinNoise(Point(-84.4207, 190.346, 139.02)), 0.01);
  EXPECT_NEAR(0.16357, PerlinNoise(Point(-82.9689, -226.675, -49.3199)), 0.01);
  EXPECT_NEAR(-0.107677, PerlinNoise(Point(-82.4815, 164.245, 246.998)), 0.01);
  EXPECT_NEAR(0.50119, PerlinNoise(Point(-81.9365, -179.457, 223.97)), 0.01);
  EXPECT_NEAR(-0.133313, PerlinNoise(Point(-79.9613, -49.2905, 249.407)), 0.01);
  EXPECT_NEAR(0.378445, PerlinNoise(Point(-75.5857, -153.452, 225.207)), 0.01);
  EXPECT_NEAR(-0.134718, PerlinNoise(Point(-75.1406, 116.119, 125.302)), 0.01);
  EXPECT_NEAR(0.35784, PerlinNoise(Point(-74.3175, 200.37, -249.933)), 0.01);
  EXPECT_NEAR(0.0149074, PerlinNoise(Point(-73.1322, -167.211, 75.7946)), 0.01);
  EXPECT_NEAR(0.0237535, PerlinNoise(Point(-71.8431, -169.287, 94.1576)), 0.01);
  EXPECT_NEAR(-0.112732, PerlinNoise(Point(-70.4942, 13.608, 226.27)), 0.01);
  EXPECT_NEAR(0.146573, PerlinNoise(Point(-63.5863, -155.564, 254.352)), 0.01);
  EXPECT_NEAR(-0.0543269, PerlinNoise(Point(-62.2348, -75.1353, 127.732)),
              0.01);
  EXPECT_NEAR(0.0849462, PerlinNoise(Point(-59.0903, 124.36, -158.813)), 0.01);
  EXPECT_NEAR(0.017734, PerlinNoise(Point(-56.7628, -217.536, -222.264)), 0.01);
  EXPECT_NEAR(0.312551, PerlinNoise(Point(-56.5391, 23.4278, 223.866)), 0.01);
  EXPECT_NEAR(0.496053, PerlinNoise(Point(-55.9092, 96.4452, -74.0487)), 0.01);
  EXPECT_NEAR(-0.106845, PerlinNoise(Point(-53.5145, 73.3934, 173.926)), 0.01);
  EXPECT_NEAR(0.173463, PerlinNoise(Point(-52.0619, -128.886, -191.332)), 0.01);
  EXPECT_NEAR(-0.451124, PerlinNoise(Point(-51.8354, -150.662, -51.9608)),
              0.01);
  EXPECT_NEAR(-0.256462, PerlinNoise(Point(-48.27, 97.9253, 15.4015)), 0.01);
  EXPECT_NEAR(-0.00376495, PerlinNoise(Point(-47.2695, -181.155, -197.958)),
              0.01);
  EXPECT_NEAR(-0.394693, PerlinNoise(Point(-46.8107, -56.6313, 3.38908)), 0.01);
  EXPECT_NEAR(0.22322, PerlinNoise(Point(-46.7062, 187.439, 100.047)), 0.01);
  EXPECT_NEAR(-0.0973498, PerlinNoise(Point(-45.0489, -234.127, -219.4)), 0.01);
  EXPECT_NEAR(0.357636, PerlinNoise(Point(-45.352, -207.095, 128.059)), 0.01);
  EXPECT_NEAR(-0.159015, PerlinNoise(Point(-45.1922, 214.297, 185.055)), 0.01);
  EXPECT_NEAR(0.0429758, PerlinNoise(Point(-44.3367, -196.435, -54.7204)),
              0.01);
  EXPECT_NEAR(0.137946, PerlinNoise(Point(-44.7075, -17.3699, 201.491)), 0.01);
  EXPECT_NEAR(-0.00418752, PerlinNoise(Point(-44.5663, 12.1168, 45.8854)),
              0.01);
  EXPECT_NEAR(-0.186534, PerlinNoise(Point(-43.9253, -143.319, -238.957)),
              0.01);
  EXPECT_NEAR(-0.114952, PerlinNoise(Point(-42.2817, 152.347, -78.5417)), 0.01);
  EXPECT_NEAR(0.22044, PerlinNoise(Point(-41.4067, -70.175, 107.819)), 0.01);
  EXPECT_NEAR(0.148805, PerlinNoise(Point(-41.994, -54.2031, 84.0306)), 0.01);
  EXPECT_NEAR(-0.361116, PerlinNoise(Point(-40.4271, -63.5449, -128.819)),
              0.01);
  EXPECT_NEAR(-0.399075, PerlinNoise(Point(-38.742, 196.256, 174.231)), 0.01);
  EXPECT_NEAR(0.323062, PerlinNoise(Point(-37.1788, 112.711, -35.4169)), 0.01);
  EXPECT_NEAR(0.398251, PerlinNoise(Point(-36.279, -232.531, -220.016)), 0.01);
  EXPECT_NEAR(-0.243756, PerlinNoise(Point(-35.6342, -199.982, -112.081)),
              0.01);
  EXPECT_NEAR(-0.198776, PerlinNoise(Point(-35.9492, -127.293, -199.702)),
              0.01);
  EXPECT_NEAR(-0.24395, PerlinNoise(Point(-34.7414, -248.068, 33.356)), 0.01);
  EXPECT_NEAR(-0.119997, PerlinNoise(Point(-34.5269, 141.41, -59.4678)), 0.01);
  EXPECT_NEAR(-0.0414903, PerlinNoise(Point(-30.1759, -119.873, 94.8786)),
              0.01);
  EXPECT_NEAR(0.0736189, PerlinNoise(Point(-30.3674, 27.7388, -8.57221)), 0.01);
  EXPECT_NEAR(-0.300553, PerlinNoise(Point(-29.6803, -177.181, 161.901)), 0.01);
  EXPECT_NEAR(0.023658, PerlinNoise(Point(-28.8651, -127.009, -48.4451)), 0.01);
  EXPECT_NEAR(-0.319146, PerlinNoise(Point(-26.9363, -88.3101, -18.2103)),
              0.01);
  EXPECT_NEAR(-0.23698, PerlinNoise(Point(-26.7199, -27.0489, 65.5678)), 0.01);
  EXPECT_NEAR(-0.0793062, PerlinNoise(Point(-26.4338, 62.3717, 102.161)), 0.01);
  EXPECT_NEAR(0.0388115, PerlinNoise(Point(-24.8668, -246.834, 83.1212)), 0.01);
  EXPECT_NEAR(0.229824, PerlinNoise(Point(-24.4224, -120.652, -169.926)), 0.01);
  EXPECT_NEAR(-0.460676, PerlinNoise(Point(-24.3897, 51.6407, 134.559)), 0.01);
  EXPECT_NEAR(0.0843165, PerlinNoise(Point(-24.8219, 128.087, -244.997)), 0.01);
  EXPECT_NEAR(-0.0212954, PerlinNoise(Point(-24.1095, 178.656, 174.849)), 0.01);
  EXPECT_NEAR(-0.296882, PerlinNoise(Point(-23.4059, -122.01, 242.804)), 0.01);
  EXPECT_NEAR(0.076608, PerlinNoise(Point(-22.4117, 162.279, -119.092)), 0.01);
  EXPECT_NEAR(0.0562845, PerlinNoise(Point(-20.2455, -97.0856, 173.817)), 0.01);
  EXPECT_NEAR(0.254145, PerlinNoise(Point(-20.4828, -15.0432, 225.548)), 0.01);
  EXPECT_NEAR(0.11112, PerlinNoise(Point(-17.5235, 82.142, -27.9506)), 0.01);
  EXPECT_NEAR(0.114947, PerlinNoise(Point(-16.031, -252.867, 182.037)), 0.01);
  EXPECT_NEAR(-0.295961, PerlinNoise(Point(-16.6034, 54.8943, 3.61374)), 0.01);
  EXPECT_NEAR(0.223059, PerlinNoise(Point(-16.9567, 115.914, 16.747)), 0.01);
  EXPECT_NEAR(-0.131675, PerlinNoise(Point(-15.0369, -110.894, -166.037)),
              0.01);
  EXPECT_NEAR(-0.223698, PerlinNoise(Point(-15.5808, 107.398, -163.595)), 0.01);
  EXPECT_NEAR(-0.0843803, PerlinNoise(Point(-15.5033, 205.98, -30.1551)), 0.01);
  EXPECT_NEAR(-0.00111373, PerlinNoise(Point(-15.7238, 252.808, 206.154)),
              0.01);
  EXPECT_NEAR(0.329756, PerlinNoise(Point(-14.7787, -147.531, -200.405)), 0.01);
  EXPECT_NEAR(0.0557417, PerlinNoise(Point(-14.8471, 206.223, -220.219)), 0.01);
  EXPECT_NEAR(-0.110327, PerlinNoise(Point(-13.4683, -74.2874, -11.4689)),
              0.01);
  EXPECT_NEAR(0.00140472, PerlinNoise(Point(-13.827, -31.0927, -99.9065)),
              0.01);
  EXPECT_NEAR(-0.476783, PerlinNoise(Point(-12.766, -251.253, 200.643)), 0.01);
  EXPECT_NEAR(0.253415, PerlinNoise(Point(-12.1108, 43.2822, 104.097)), 0.01);
  EXPECT_NEAR(-0.0493673, PerlinNoise(Point(-9.18562, -230.142, 103.22)), 0.01);
  EXPECT_NEAR(-0.0988673, PerlinNoise(Point(-9.61138, -229.565, 154.766)),
              0.01);
  EXPECT_NEAR(0.233467, PerlinNoise(Point(-9.7413, 122.377, 226.529)), 0.01);
  EXPECT_NEAR(0.149148, PerlinNoise(Point(-9.00277, 217.112, -200.19)), 0.01);
  EXPECT_NEAR(0.214675, PerlinNoise(Point(-8.46805, -234.844, 79.7158)), 0.01);
  EXPECT_NEAR(0.145731, PerlinNoise(Point(-7.31193, -157.918, 39.0023)), 0.01);
  EXPECT_NEAR(-0.0380263, PerlinNoise(Point(-2.36404, -209.149, -84.1111)),
              0.01);
  EXPECT_NEAR(0.327694, PerlinNoise(Point(-1.20624, -255.003, 97.7307)), 0.01);
  EXPECT_NEAR(0.352396, PerlinNoise(Point(-1.30715, -35.0229, -154.069)), 0.01);
  EXPECT_NEAR(-0.194401, PerlinNoise(Point(-0.160421, -217.728, 119.128)),
              0.01);
  EXPECT_NEAR(-0.0860391, PerlinNoise(Point(-0.192004, -101.905, -56.7763)),
              0.01);
  EXPECT_NEAR(0.223538, PerlinNoise(Point(-0.429508, 112.467, -194.18)), 0.01);
  EXPECT_NEAR(-0.515393, PerlinNoise(Point(0.887732, 239.329, 153.941)), 0.01);
  EXPECT_NEAR(-0.225851, PerlinNoise(Point(1.21194, -1.71101, -105.603)), 0.01);
  EXPECT_NEAR(0.0595579, PerlinNoise(Point(4.13839, -104.952, 164.62)), 0.01);
  EXPECT_NEAR(-0.0602835, PerlinNoise(Point(4.46436, -72.5144, -93.8243)),
              0.01);
  EXPECT_NEAR(-0.245052, PerlinNoise(Point(5.91314, -88.81, -154.274)), 0.01);
  EXPECT_NEAR(0.495129, PerlinNoise(Point(5.39377, 244.048, -242.134)), 0.01);
  EXPECT_NEAR(-0.242071, PerlinNoise(Point(6.39038, 175.542, -50.606)), 0.01);
  EXPECT_NEAR(-0.184869, PerlinNoise(Point(7.30026, 97.6349, 42.2857)), 0.01);
  EXPECT_NEAR(-0.158065, PerlinNoise(Point(7.31774, 140.802, 208.319)), 0.01);
  EXPECT_NEAR(-0.296966, PerlinNoise(Point(8.6843, -70.4628, -175.496)), 0.01);
  EXPECT_NEAR(0.0263539, PerlinNoise(Point(9.12961, -160.152, -207.924)), 0.01);
  EXPECT_NEAR(-0.302019, PerlinNoise(Point(9.03766, -124.075, -138.803)), 0.01);
  EXPECT_NEAR(-0.135527, PerlinNoise(Point(9.9268, -33.0934, 104.504)), 0.01);
  EXPECT_NEAR(0.00563824, PerlinNoise(Point(10.4734, 92.473, -168.916)), 0.01);
  EXPECT_NEAR(0.266571, PerlinNoise(Point(11.0077, -140.771, 188.002)), 0.01);
  EXPECT_NEAR(-0.0468283, PerlinNoise(Point(11.0664, 0.30947, -186.32)), 0.01);
  EXPECT_NEAR(0.129377, PerlinNoise(Point(12.3416, 19.0739, 145.519)), 0.01);
  EXPECT_NEAR(-0.0011018, PerlinNoise(Point(13.5417, -101.232, 137.57)), 0.01);
  EXPECT_NEAR(0.0702454, PerlinNoise(Point(13.8665, -2.55098, 70.386)), 0.01);
  EXPECT_NEAR(0.231934, PerlinNoise(Point(14.8731, 92.4843, -189.517)), 0.01);
  EXPECT_NEAR(-0.385874, PerlinNoise(Point(15.5113, -246.278, -46.0997)), 0.01);
  EXPECT_NEAR(-0.367993, PerlinNoise(Point(16.7882, 248.721, 178.014)), 0.01);
  EXPECT_NEAR(-0.041189, PerlinNoise(Point(20.0787, -81.2908, 37.8009)), 0.01);
  EXPECT_NEAR(0.221462, PerlinNoise(Point(21.674, 74.3025, -253.879)), 0.01);
  EXPECT_NEAR(-0.0355708, PerlinNoise(Point(22.877, -248.735, 5.38559)), 0.01);
  EXPECT_NEAR(0.200527, PerlinNoise(Point(24.3706, 41.6065, -213.701)), 0.01);
  EXPECT_NEAR(0.0918649, PerlinNoise(Point(25.5317, -192.414, 54.9126)), 0.01);
  EXPECT_NEAR(-0.0143685, PerlinNoise(Point(25.8667, -17.1525, 138.452)), 0.01);
  EXPECT_NEAR(-0.0540535, PerlinNoise(Point(25.4278, 75.3853, -122.061)), 0.01);
  EXPECT_NEAR(0.122695, PerlinNoise(Point(27.4919, 183.809, -96.235)), 0.01);
  EXPECT_NEAR(-0.539622, PerlinNoise(Point(28.3385, 146.86, -68.2887)), 0.01);
  EXPECT_NEAR(-0.237451, PerlinNoise(Point(31.1714, -182.113, -125.468)), 0.01);
  EXPECT_NEAR(0.182951, PerlinNoise(Point(32.0447, 240.115, -165.176)), 0.01);
  EXPECT_NEAR(-0.28012, PerlinNoise(Point(33.4419, 206.928, -65.2397)), 0.01);
  EXPECT_NEAR(0.547226, PerlinNoise(Point(35.5527, -199.13, -146.557)), 0.01);
  EXPECT_NEAR(0.0846507, PerlinNoise(Point(35.8398, -199.189, -79.9752)), 0.01);
  EXPECT_NEAR(0.276726, PerlinNoise(Point(36.3743, 184.008, 25.9949)), 0.01);
  EXPECT_NEAR(0.236194, PerlinNoise(Point(38.5842, -189.239, 57.3075)), 0.01);
  EXPECT_NEAR(-0.572615, PerlinNoise(Point(39.6809, -238.336, 96.4733)), 0.01);
  EXPECT_NEAR(-0.0289279, PerlinNoise(Point(39.9312, 129.775, -98.3114)), 0.01);
  EXPECT_NEAR(0.265992, PerlinNoise(Point(40.4918, -167.734, -186.555)), 0.01);
  EXPECT_NEAR(-0.141285, PerlinNoise(Point(40.9869, -47.7055, -84.1016)), 0.01);
  EXPECT_NEAR(0.0676555, PerlinNoise(Point(40.0169, -9.89788, 63.5417)), 0.01);
  EXPECT_NEAR(0.0530835, PerlinNoise(Point(40.0603, 188.286, -86.5374)), 0.01);
  EXPECT_NEAR(0.15577, PerlinNoise(Point(41.8982, -121.275, 252.273)), 0.01);
  EXPECT_NEAR(0.259013, PerlinNoise(Point(41.4766, 164.522, -100.357)), 0.01);
  EXPECT_NEAR(-0.0964575, PerlinNoise(Point(43.8691, -71.2631, 3.40568)), 0.01);
  EXPECT_NEAR(-0.174826, PerlinNoise(Point(45.1408, 90.403, -92.0339)), 0.01);
  EXPECT_NEAR(-0.00425614, PerlinNoise(Point(45.2127, 247.272, 32.1513)), 0.01);
  EXPECT_NEAR(0.134853, PerlinNoise(Point(46.2698, -167.993, -218.915)), 0.01);
  EXPECT_NEAR(-0.262286, PerlinNoise(Point(47.843, -110.061, -49.8591)), 0.01);
  EXPECT_NEAR(0.279111, PerlinNoise(Point(48.9535, -100.467, 204.354)), 0.01);
  EXPECT_NEAR(0.284257, PerlinNoise(Point(50.0646, -211.859, 202.554)), 0.01);
  EXPECT_NEAR(0.146741, PerlinNoise(Point(50.7182, -208.194, 80.0488)), 0.01);
  EXPECT_NEAR(0.174577, PerlinNoise(Point(50.6148, -125.507, -161.245)), 0.01);
  EXPECT_NEAR(0.167827, PerlinNoise(Point(54.649, 162.331, -94.1409)), 0.01);
  EXPECT_NEAR(0.00838893, PerlinNoise(Point(56.7439, 117.523, -129.397)), 0.01);
  EXPECT_NEAR(-0.379533, PerlinNoise(Point(57.4932, -36.4505, -189.703)), 0.01);
  EXPECT_NEAR(-0.108344, PerlinNoise(Point(57.9609, -8.45629, 168.781)), 0.01);
  EXPECT_NEAR(0.467818, PerlinNoise(Point(58.5761, -49.2629, -252.658)), 0.01);
  EXPECT_NEAR(0.0985427, PerlinNoise(Point(58.6495, -46.8239, -108.459)), 0.01);
  EXPECT_NEAR(0.428878, PerlinNoise(Point(59.801, -150.123, 17.1863)), 0.01);
  EXPECT_NEAR(0.500445, PerlinNoise(Point(59.9637, 211.583, -187.28)), 0.01);
  EXPECT_NEAR(-0.252192, PerlinNoise(Point(60.6602, 171.876, 61.0688)), 0.01);
  EXPECT_NEAR(-0.0975485, PerlinNoise(Point(63.2877, -216.755, 167.238)), 0.01);
  EXPECT_NEAR(0.347031, PerlinNoise(Point(63.0576, -73.8441, -16.2486)), 0.01);
  EXPECT_NEAR(0.356304, PerlinNoise(Point(63.7013, -7.55677, -252.279)), 0.01);
  EXPECT_NEAR(-0.142302, PerlinNoise(Point(64.1335, 105.029, 101.813)), 0.01);
  EXPECT_NEAR(-0.141126, PerlinNoise(Point(66.4852, 75.8762, 38.1918)), 0.01);
  EXPECT_NEAR(0.103698, PerlinNoise(Point(66.5543, 84.2096, -90.3044)), 0.01);
  EXPECT_NEAR(0.314178, PerlinNoise(Point(66.9481, 165.875, -47.3192)), 0.01);
  EXPECT_NEAR(0.0744116, PerlinNoise(Point(66.7709, 192.601, 112.201)), 0.01);
  EXPECT_NEAR(0.115005, PerlinNoise(Point(68.3502, -113.593, 137.228)), 0.01);
  EXPECT_NEAR(0.186433, PerlinNoise(Point(70.7938, -217.427, 51.7715)), 0.01);
  EXPECT_NEAR(-0.00608526, PerlinNoise(Point(72.1754, 28.9721, -1.02127)),
              0.01);
  EXPECT_NEAR(-0.0934319, PerlinNoise(Point(73.4287, 136.641, -65.9609)), 0.01);
  EXPECT_NEAR(0.23882, PerlinNoise(Point(75.249, 106.642, -12.7542)), 0.01);
  EXPECT_NEAR(-0.124992, PerlinNoise(Point(76.4715, -136.482, 112.533)), 0.01);
  EXPECT_NEAR(0.581622, PerlinNoise(Point(76.9985, -111.096, 107.439)), 0.01);
  EXPECT_NEAR(0.164275, PerlinNoise(Point(77.9607, 156.829, -46.191)), 0.01);
  EXPECT_NEAR(0.161814, PerlinNoise(Point(78.9861, -60.6757, 200.536)), 0.01);
  EXPECT_NEAR(-0.49438, PerlinNoise(Point(78.5412, 233.724, 9.23001)), 0.01);
  EXPECT_NEAR(0.232821, PerlinNoise(Point(79.3431, -242.243, -210.146)), 0.01);
  EXPECT_NEAR(0.244643, PerlinNoise(Point(79.6397, 162.71, 61.8312)), 0.01);
  EXPECT_NEAR(-0.386105, PerlinNoise(Point(80.1179, -135.611, -61.0586)), 0.01);
  EXPECT_NEAR(-0.0908089, PerlinNoise(Point(80.1637, 75.6246, -191.391)), 0.01);
  EXPECT_NEAR(0.0264953, PerlinNoise(Point(80.8368, 77.3023, 55.8146)), 0.01);
  EXPECT_NEAR(-0.391346, PerlinNoise(Point(81.0684, -52.4142, -252.543)), 0.01);
  EXPECT_NEAR(-0.0537621, PerlinNoise(Point(82.3365, 119.002, -58.1908)), 0.01);
  EXPECT_NEAR(0.585732, PerlinNoise(Point(82.8734, 249.061, -136.526)), 0.01);
  EXPECT_NEAR(-0.1744, PerlinNoise(Point(83.1561, 236.504, -247.778)), 0.01);
  EXPECT_NEAR(-0.0248772, PerlinNoise(Point(85.7151, 243.773, -128.836)), 0.01);
  EXPECT_NEAR(-0.119793, PerlinNoise(Point(86.2258, -139.16, -3.5121)), 0.01);
  EXPECT_NEAR(-0.0853174, PerlinNoise(Point(87.2056, 217.936, 239.932)), 0.01);
  EXPECT_NEAR(0.499074, PerlinNoise(Point(90.4915, -212.074, -174.426)), 0.01);
  EXPECT_NEAR(0.36294, PerlinNoise(Point(90.9085, -155.924, 14.7584)), 0.01);
  EXPECT_NEAR(0.16717, PerlinNoise(Point(92.7396, -197.096, -40.4212)), 0.01);
  EXPECT_NEAR(-0.166857, PerlinNoise(Point(95.5706, 134.941, 151.541)), 0.01);
  EXPECT_NEAR(-0.297429, PerlinNoise(Point(96.4046, 164.263, 11.861)), 0.01);
  EXPECT_NEAR(-0.232575, PerlinNoise(Point(97.9768, 184.766, 144.746)), 0.01);
  EXPECT_NEAR(0.000619234, PerlinNoise(Point(99.7145, 4.18574, -125.099)),
              0.01);
  EXPECT_NEAR(-0.606755, PerlinNoise(Point(99.0641, 57.1748, 5.41006)), 0.01);
  EXPECT_NEAR(-0.00599822, PerlinNoise(Point(99.4657, 227.642, -77.8896)),
              0.01);
  EXPECT_NEAR(-0.337354, PerlinNoise(Point(100.493, 52.1443, 224.894)), 0.01);
  EXPECT_NEAR(0.212083, PerlinNoise(Point(101.261, 244.487, 254.735)), 0.01);
  EXPECT_NEAR(-0.171394, PerlinNoise(Point(102.823, -243.232, 174.692)), 0.01);
  EXPECT_NEAR(0.0455576, PerlinNoise(Point(102.367, -199.368, -182.169)), 0.01);
  EXPECT_NEAR(-0.226616, PerlinNoise(Point(102.409, -4.10028, 96.6)), 0.01);
  EXPECT_NEAR(-0.121789, PerlinNoise(Point(102.45, 134.926, -115.89)), 0.01);
  EXPECT_NEAR(0.114924, PerlinNoise(Point(103.008, -83.5134, 21.8748)), 0.01);
  EXPECT_NEAR(-0.10298, PerlinNoise(Point(103.313, 49.4609, -26.8758)), 0.01);
  EXPECT_NEAR(0.0543866, PerlinNoise(Point(103.572, 195.932, 134.715)), 0.01);
  EXPECT_NEAR(0.260015, PerlinNoise(Point(105.631, -130.501, -35.5674)), 0.01);
  EXPECT_NEAR(-0.2567, PerlinNoise(Point(105.744, 101.943, -220.3)), 0.01);
  EXPECT_NEAR(0.484147, PerlinNoise(Point(108.274, -81.472, -33.0205)), 0.01);
  EXPECT_NEAR(0.0233024, PerlinNoise(Point(108.949, 39.0169, -157.192)), 0.01);
  EXPECT_NEAR(0.417132, PerlinNoise(Point(108.187, 165.104, 222.395)), 0.01);
  EXPECT_NEAR(-0.274298, PerlinNoise(Point(111.119, 254.249, 188.988)), 0.01);
  EXPECT_NEAR(0.393377, PerlinNoise(Point(114.787, 142.344, -173.039)), 0.01);
  EXPECT_NEAR(-0.225265, PerlinNoise(Point(115.763, -56.1204, 224.091)), 0.01);
  EXPECT_NEAR(0.100609, PerlinNoise(Point(115.517, 159.64, -168.19)), 0.01);
  EXPECT_NEAR(0.0157252, PerlinNoise(Point(119.178, -229.734, 195.552)), 0.01);
  EXPECT_NEAR(0.127103, PerlinNoise(Point(121.65, -255.339, 15.7729)), 0.01);
  EXPECT_NEAR(-0.106519, PerlinNoise(Point(121.409, -150.799, 9.38794)), 0.01);
  EXPECT_NEAR(0.214832, PerlinNoise(Point(121.49, -135.346, -84.0412)), 0.01);
  EXPECT_NEAR(-0.199556, PerlinNoise(Point(121.072, -8.52056, 8.39261)), 0.01);
  EXPECT_NEAR(-0.45154, PerlinNoise(Point(122.434, -202.382, 237.245)), 0.01);
  EXPECT_NEAR(0.323968, PerlinNoise(Point(122.533, -133.027, 66.2923)), 0.01);
  EXPECT_NEAR(0.598814, PerlinNoise(Point(122.264, 34.612, 5.21072)), 0.01);
  EXPECT_NEAR(-0.17984, PerlinNoise(Point(122.645, 111.486, -121.4)), 0.01);
  EXPECT_NEAR(0.0114164, PerlinNoise(Point(122.364, 122.602, -127.813)), 0.01);
  EXPECT_NEAR(0.235181, PerlinNoise(Point(123.6, 224.02, -165.358)), 0.01);
  EXPECT_NEAR(0.504221, PerlinNoise(Point(125.358, -242.067, -226.095)), 0.01);
  EXPECT_NEAR(-0.162808, PerlinNoise(Point(127.6, 157.892, 81.5146)), 0.01);
  EXPECT_NEAR(0.510895, PerlinNoise(Point(128.267, -119.057, 133.346)), 0.01);
  EXPECT_NEAR(-0.341385, PerlinNoise(Point(128.034, 92.3068, 38.7828)), 0.01);
  EXPECT_NEAR(0.285315, PerlinNoise(Point(129.244, -76.5373, 218.068)), 0.01);
  EXPECT_NEAR(-0.227781, PerlinNoise(Point(129.286, 127.498, -7.29864)), 0.01);
  EXPECT_NEAR(-0.0580043, PerlinNoise(Point(133.076, 218.376, -34.1305)), 0.01);
  EXPECT_NEAR(0.0546307, PerlinNoise(Point(135.102, 61.3696, -239.897)), 0.01);
  EXPECT_NEAR(0.455306, PerlinNoise(Point(135.781, 176.177, -151.527)), 0.01);
  EXPECT_NEAR(-0.225276, PerlinNoise(Point(136.931, -93.6673, -247.807)), 0.01);
  EXPECT_NEAR(-0.140541, PerlinNoise(Point(136.757, -33.2902, -107.959)), 0.01);
  EXPECT_NEAR(0.167576, PerlinNoise(Point(136.528, 178.736, -36.319)), 0.01);
  EXPECT_NEAR(0.0280822, PerlinNoise(Point(136.287, 232.001, 94.1449)), 0.01);
  EXPECT_NEAR(0.207464, PerlinNoise(Point(137.498, -98.6313, -29.9451)), 0.01);
  EXPECT_NEAR(0.0355747, PerlinNoise(Point(137.98, -67.193, 76.5797)), 0.01);
  EXPECT_NEAR(-0.415611, PerlinNoise(Point(137.181, 26.9356, 98.4947)), 0.01);
  EXPECT_NEAR(-0.116859, PerlinNoise(Point(138.625, -192.104, 170.89)), 0.01);
  EXPECT_NEAR(-0.0655736, PerlinNoise(Point(139.675, -131.429, 160.895)), 0.01);
  EXPECT_NEAR(-0.53918, PerlinNoise(Point(139.547, 1.24024, 29.978)), 0.01);
  EXPECT_NEAR(0.696819, PerlinNoise(Point(142.266, 88.4972, -61.7818)), 0.01);
  EXPECT_NEAR(0.0997092, PerlinNoise(Point(142.458, 142.307, 88.6837)), 0.01);
  EXPECT_NEAR(0.0372028, PerlinNoise(Point(150.584, 39.6075, -162.773)), 0.01);
  EXPECT_NEAR(0.202734, PerlinNoise(Point(153.048, -247.686, 209.408)), 0.01);
  EXPECT_NEAR(0.0548081, PerlinNoise(Point(153.094, 199.247, 5.32136)), 0.01);
  EXPECT_NEAR(0.405079, PerlinNoise(Point(157.064, 153.768, -101.51)), 0.01);
  EXPECT_NEAR(-0.210355, PerlinNoise(Point(158.93, 107.337, 40.7388)), 0.01);
  EXPECT_NEAR(-0.296446, PerlinNoise(Point(160.327, 136.787, -85.0907)), 0.01);
  EXPECT_NEAR(-0.246749, PerlinNoise(Point(162.701, 52.2072, -240.983)), 0.01);
  EXPECT_NEAR(-0.148636, PerlinNoise(Point(163.656, -65.4785, -253.807)), 0.01);
  EXPECT_NEAR(-0.330882, PerlinNoise(Point(163.474, 175.136, 138.391)), 0.01);
  EXPECT_NEAR(0.163789, PerlinNoise(Point(165.944, -69.7646, 82.848)), 0.01);
  EXPECT_NEAR(-0.592871, PerlinNoise(Point(165.71, 36.4944, 191.676)), 0.01);
  EXPECT_NEAR(-0.39089, PerlinNoise(Point(168.573, -228.662, 84.0208)), 0.01);
  EXPECT_NEAR(0.342033, PerlinNoise(Point(169.068, -188.002, -139.718)), 0.01);
  EXPECT_NEAR(-0.200296, PerlinNoise(Point(170.432, 52.6225, 0.156742)), 0.01);
  EXPECT_NEAR(0.446263, PerlinNoise(Point(171.462, -232.918, 157.999)), 0.01);
  EXPECT_NEAR(-0.0635595, PerlinNoise(Point(172.543, -245.62, 188.825)), 0.01);
  EXPECT_NEAR(0.176184, PerlinNoise(Point(172.122, -138.114, -163.731)), 0.01);
  EXPECT_NEAR(0.430417, PerlinNoise(Point(172.621, 116.417, 76.6115)), 0.01);
  EXPECT_NEAR(-0.533745, PerlinNoise(Point(173.766, -35.3757, 192.2)), 0.01);
  EXPECT_NEAR(-0.441484, PerlinNoise(Point(173.138, -29.2475, -210.327)), 0.01);
  EXPECT_NEAR(-0.293314, PerlinNoise(Point(173.802, 48.9146, -129.877)), 0.01);
  EXPECT_NEAR(0.151721, PerlinNoise(Point(174.483, -97.8232, 33.1369)), 0.01);
  EXPECT_NEAR(0.278754, PerlinNoise(Point(175.707, 7.78618, -171.027)), 0.01);
  EXPECT_NEAR(0.0693602, PerlinNoise(Point(180.9, -44.6324, -181.088)), 0.01);
  EXPECT_NEAR(-0.232537, PerlinNoise(Point(180.147, 8.92212, -239.806)), 0.01);
  EXPECT_NEAR(-0.22676, PerlinNoise(Point(180.587, 43.0575, 230.22)), 0.01);
  EXPECT_NEAR(0.14899, PerlinNoise(Point(180.881, 202.155, -227.961)), 0.01);
  EXPECT_NEAR(0.399252, PerlinNoise(Point(181.002, 246.785, 230.405)), 0.01);
  EXPECT_NEAR(-0.114876, PerlinNoise(Point(183.117, -158.611, 181.988)), 0.01);
  EXPECT_NEAR(0.126282, PerlinNoise(Point(187.286, 201.29, 209.23)), 0.01);
  EXPECT_NEAR(-0.0600376, PerlinNoise(Point(188.679, 95.7383, 90.5026)), 0.01);
  EXPECT_NEAR(0.0710164, PerlinNoise(Point(189.023, -118.184, 243.013)), 0.01);
  EXPECT_NEAR(0.178141, PerlinNoise(Point(189.931, -110.212, -229.895)), 0.01);
  EXPECT_NEAR(-0.0842039, PerlinNoise(Point(189.914, 7.85296, -167.064)), 0.01);
  EXPECT_NEAR(0.217287, PerlinNoise(Point(190.801, -76.8904, -41.7363)), 0.01);
  EXPECT_NEAR(-0.229387, PerlinNoise(Point(190.245, -37.9879, 108.905)), 0.01);
  EXPECT_NEAR(0.321677, PerlinNoise(Point(190.59, 21.3971, -127.918)), 0.01);
  EXPECT_NEAR(-0.122889, PerlinNoise(Point(193.747, 157.433, 140.553)), 0.01);
  EXPECT_NEAR(-0.0506116, PerlinNoise(Point(196.124, -124.807, -35.6998)),
              0.01);
  EXPECT_NEAR(0.128902, PerlinNoise(Point(196.164, 228.971, -45.3181)), 0.01);
  EXPECT_NEAR(0.264173, PerlinNoise(Point(197.964, -133.362, -81.1757)), 0.01);
  EXPECT_NEAR(0.332873, PerlinNoise(Point(198.554, -154.091, -170.943)), 0.01);
  EXPECT_NEAR(-0.402029, PerlinNoise(Point(201.246, -165.545, -235.951)), 0.01);
  EXPECT_NEAR(0.193288, PerlinNoise(Point(201.534, -104.862, -47.4045)), 0.01);
  EXPECT_NEAR(-0.418568, PerlinNoise(Point(202.55, 235.524, -123.935)), 0.01);
  EXPECT_NEAR(0.17402, PerlinNoise(Point(203.476, 46.0597, 236.493)), 0.01);
  EXPECT_NEAR(-0.408481, PerlinNoise(Point(204.257, -202.829, 191.274)), 0.01);
  EXPECT_NEAR(-0.508124, PerlinNoise(Point(204.698, -145.204, 38.1029)), 0.01);
  EXPECT_NEAR(-0.0717056, PerlinNoise(Point(209.562, -197.146, 208.024)), 0.01);
  EXPECT_NEAR(0.0486658, PerlinNoise(Point(209.446, 106.473, 23.2607)), 0.01);
  EXPECT_NEAR(-0.24524, PerlinNoise(Point(211.999, 115.912, -4.51557)), 0.01);
  EXPECT_NEAR(-0.078417, PerlinNoise(Point(212.528, -225.004, 195.518)), 0.01);
  EXPECT_NEAR(0.147888, PerlinNoise(Point(212.894, -98.7073, -113.541)), 0.01);
  EXPECT_NEAR(0.341849, PerlinNoise(Point(213.678, -11.3658, -214.227)), 0.01);
  EXPECT_NEAR(-0.0915576, PerlinNoise(Point(214.503, -14.1106, -43.6126)),
              0.01);
  EXPECT_NEAR(-0.183056, PerlinNoise(Point(214.478, 39.8597, -185.9)), 0.01);
  EXPECT_NEAR(0.41466, PerlinNoise(Point(214.632, 213.203, 29.5492)), 0.01);
  EXPECT_NEAR(-0.161495, PerlinNoise(Point(216.224, 147.636, 158.047)), 0.01);
  EXPECT_NEAR(0.254613, PerlinNoise(Point(216.828, 186.691, 16.5859)), 0.01);
  EXPECT_NEAR(0.195955, PerlinNoise(Point(216.554, 188.902, -43.6368)), 0.01);
  EXPECT_NEAR(-0.0453197, PerlinNoise(Point(217.917, 70.3096, -113.195)), 0.01);
  EXPECT_NEAR(-0.0680384, PerlinNoise(Point(218.095, -117.175, -69.8159)),
              0.01);
  EXPECT_NEAR(0.506581, PerlinNoise(Point(218.48, 69.06, -178.686)), 0.01);
  EXPECT_NEAR(-0.327524, PerlinNoise(Point(220.275, 45.0208, 165.995)), 0.01);
  EXPECT_NEAR(-0.586859, PerlinNoise(Point(222.792, 171.834, 166.566)), 0.01);
  EXPECT_NEAR(0.141171, PerlinNoise(Point(224.763, -158.308, 105.603)), 0.01);
  EXPECT_NEAR(0.420689, PerlinNoise(Point(225.535, -104.502, -254.538)), 0.01);
  EXPECT_NEAR(0.128731, PerlinNoise(Point(226.317, -125.664, -37.8443)), 0.01);
  EXPECT_NEAR(-0.351574, PerlinNoise(Point(228.118, -96.6291, -6.43098)), 0.01);
  EXPECT_NEAR(-0.0833402, PerlinNoise(Point(228.961, 154.178, 136.117)), 0.01);
  EXPECT_NEAR(-0.133502, PerlinNoise(Point(229.155, -39.9662, 38.135)), 0.01);
  EXPECT_NEAR(0.261383, PerlinNoise(Point(230.458, -125.66, -52.5921)), 0.01);
  EXPECT_NEAR(0.0097167, PerlinNoise(Point(231.044, -241.346, 201.382)), 0.01);
  EXPECT_NEAR(-0.349393, PerlinNoise(Point(233.695, -164.538, 165.539)), 0.01);
  EXPECT_NEAR(0.14972, PerlinNoise(Point(233.408, 60.0465, 8.91542)), 0.01);
  EXPECT_NEAR(-0.0306724, PerlinNoise(Point(235.918, 156.504, -210.221)), 0.01);
  EXPECT_NEAR(0.296604, PerlinNoise(Point(240.07, 16.463, -19.3659)), 0.01);
  EXPECT_NEAR(-0.0758202, PerlinNoise(Point(240.01, 174.368, -75.46)), 0.01);
  EXPECT_NEAR(-0.657483, PerlinNoise(Point(241.836, -121.648, -254.692)), 0.01);
  EXPECT_NEAR(-0.0602868, PerlinNoise(Point(241.184, 243.862, -40.369)), 0.01);
  EXPECT_NEAR(0.50422, PerlinNoise(Point(242.207, 229.082, 19.4461)), 0.01);
  EXPECT_NEAR(0.0465298, PerlinNoise(Point(245.805, -97.2163, -218.262)), 0.01);
  EXPECT_NEAR(-0.272785, PerlinNoise(Point(249.244, 81.9124, -88.9826)), 0.01);
  EXPECT_NEAR(-0.0242889, PerlinNoise(Point(252.929, 53.6181, -148.049)), 0.01);
  EXPECT_NEAR(-0.0021903, PerlinNoise(Point(253.853, 154.56, 29.4296)), 0.01);
  EXPECT_NEAR(-0.214774, PerlinNoise(Point(253.29, 200.714, 84.714)), 0.01);
}

}  // namespace
}  // namespace internal
}  // namespace textures
}  // namespace iris
