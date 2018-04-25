#include <ElCurve.h>
#include <gcrypt.h>
#include <fstream>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#define chosenHashFunction GCRY_MD_SHA512

uint8_t MAX_ZZ_BYTES, it;
uint16_t p_bits;
ZZ p, n, a, b, Gx, Gy, kU, r, e, dU, tmpX, tmpY;
ECpoint *theGenerator, *QCA, *RU, *QU, *PU;
FILE *fp;
std::string tmp_str, fifoFileName;
std::ofstream writeFile;
std::ifstream readFile;
char *buffer1, *buffer2, tmp_c;

int main(int argc, char const *argv[]) {
  if(!gcry_check_version(GCRYPT_VERSION)) {
    fputs ("libgcrypt version mismatch\n", stderr);
    exit(2);
  }

  SetSeed(RandomBnd(conv<ZZ>("9409250011798120900227")));

  readFile.open("CApublicFile.txt");
  if(!readFile.is_open()) {
    std::cout << "Couldn't open desired file!" << "\n";
    return 0;
  }
  std::getline(readFile, tmp_str);

  p_bits = atoi(tmp_str.c_str());

  switch(p_bits) {
    case 192:
      p = conv<ZZ>("6277101735386680763835789423207666416083908700390324961279");
      n = conv<ZZ>("6277101735386680763835789423176059013767194773182842284081");
      a = p-3;
      b = conv<ZZ>("2455155546008943817740293915197451784769108058161191238065");
      Gx = conv<ZZ>("602046282375688656758213480587526111916698976636884684818");
      Gy = conv<ZZ>("174050332293622031404857552280219410364023488927386650641");
      MAX_ZZ_BYTES = 24;
      break;
    case 224:
      p = conv<ZZ>("26959946667150639794667015087019630673557916260026308143510066298881");
      n = conv<ZZ>("26959946667150639794667015087019625940457807714424391721682722368061");
      a = p-3;
      b = conv<ZZ>("18958286285566608000408668544493926415504680968679321075787234672564");
      Gx = conv<ZZ>("19277929113566293071110308034699488026831934219452440156649784352033");
      Gy = conv<ZZ>("19926808758034470970197974370888749184205991990603949537637343198772");
      MAX_ZZ_BYTES = 28;
      break;
    case 256:
      p = conv<ZZ>("115792089210356248762697446949407573530086143415290314195533631308867097853951");
      n = conv<ZZ>("115792089210356248762697446949407573529996955224135760342422259061068512044369");
      a = p-3;
      b = conv<ZZ>("41058363725152142129326129780047268409114441015993725554835256314039467401291");
      Gx = conv<ZZ>("48439561293906451759052585252797914202762949526041747995844080717082404635286");
      Gy = conv<ZZ>("36134250956749795798585127919587881956611106672985015071877198253568414405109");
      MAX_ZZ_BYTES = 32;
      break;
    case 384:
      p = conv<ZZ>("39402006196394479212279040100143613805079739270465446667948293404245721771496870329047266088258938001861606973112319");
      n = conv<ZZ>("39402006196394479212279040100143613805079739270465446667946905279627659399113263569398956308152294913554433653942643");
      a = p-3;
      b = conv<ZZ>("27580193559959705877849011840389048093056905856361568521428707301988689241309860865136260764883745107765439761230575");
      Gx = conv<ZZ>("26247035095799689268623156744566981891852923491109213387815615900925518854738050089022388053975719786650872476732087");
      Gy = conv<ZZ>("8325710961489029985546751289520108179287853048861315594709205902480503199884419224438643760392947333078086511627871");
      MAX_ZZ_BYTES = 48;
      break;
    case 521:
      p = conv<ZZ>("6864797660130609714981900799081393217269435300143305409394463459185543183397656052122559640661454554977296311391480858037121987999716643812574028291115057151");
      n = conv<ZZ>("6864797660130609714981900799081393217269435300143305409394463459185543183397655394245057746333217197532963996371363321113864768612440380340372808892707005449");
      a = p-3;
      b = conv<ZZ>("1093849038073734274511112390766805569936207598951683748994586394495953116150735016013708737573759623248592132296706313309438452531591012912142327488478985984");
      Gx = conv<ZZ>("2661740802050217063228768716723360960729859168756973147706671368418802944996427808491545080627771902352094241225065558662157113545570916814161637315895999846");
      Gy = conv<ZZ>("3757180025770020463545507224491183603594455134769762486694567779615544477440556316691234405012945539562144444537289428522585666729196580810124344277578376784");
      MAX_ZZ_BYTES = 66;
      break;
    default:
      std::cout << "Choose a proper NIST elliptic curve: 192, 224, 256, 384, or 521" << std::endl;
      return 0;
  }
  ElCurve *theCurve = new ElCurve(p, a, b);
  theGenerator = new ECpoint(Gx, Gy);
  std::getline(readFile, tmp_str);
  tmpX = conv<ZZ>(tmp_str.c_str());
  std::getline(readFile, tmp_str);
  tmpY = conv<ZZ>(tmp_str.c_str());
  readFile.close();
  QCA = new ECpoint(tmpX, tmpY);
  std::cout << "Got CA public key QCA = ";
  QCA->printlnPoint();
  kU = RandomBnd(n);
  RU = theCurve->mulPoint(theGenerator, kU);

  unsigned char *RUx_str = (unsigned char *)malloc(sizeof *RUx_str * MAX_ZZ_BYTES);
  unsigned char *RUy_str = (unsigned char *)malloc(sizeof *RUy_str * MAX_ZZ_BYTES);
  //unsigned char RUx_str[MAX_ZZ_BYTES], RUy_str[MAX_ZZ_BYTES];
  BytesFromZZ(RUx_str, RU->getX(), MAX_ZZ_BYTES);
  //std::cout << RUx_str << std::endl << ZZFromBytes(RUx_str, MAX_ZZ_BYTES) << std::endl;
  BytesFromZZ(RUy_str, RU->getY(), MAX_ZZ_BYTES);
  //std::cout << RUy_str << std::endl << ZZFromBytes(RUy_str, MAX_ZZ_BYTES) << std::endl;;

  //send RU (as RUx_str and RUy_str)
  fp = fopen("mkfifoRUx", "w");
  fputs((char *)RUx_str, fp);
  fclose(fp);
  fp = fopen("mkfifoRUy", "w");
  fputs((char *)RUy_str, fp);
  fclose(fp);
  std::cout << "Sent request to CA, RU = ";
  RU->printlnPoint();

  //receive(r, CertU)
  umask(0);
  mknod("mkfifor", S_IFIFO|0666, 0);
  fp = fopen("mkfifor", "r");
  buffer1 = (char *)malloc(sizeof *buffer1 * (MAX_ZZ_BYTES + 1));
  it = 0;
  while((tmp_c = getc(fp)) != EOF) buffer1[it++] = tmp_c;
  ZZFromBytes(r, (unsigned char*)buffer1, MAX_ZZ_BYTES);
  fclose(fp);
  unlink("mkfifor");
  umask(0);
  mknod("mkfifoCertU", S_IFIFO|0666, 0);
  fp = fopen("mkfifoCertU", "r");
  buffer2 = (char *)malloc(sizeof *buffer2 * (MAX_ZZ_BYTES * 2 + 1));
  it = 0;
  while((tmp_c = getc(fp)) != EOF) buffer2[it++] = tmp_c;
  fclose(fp);
  unlink("mkfifoCertU");
  //after receiving (r,CertU) PU=Dec(CertU)
  unsigned char *e_str = (unsigned char *)malloc(sizeof *e_str * gcry_md_get_algo_dlen(chosenHashFunction));
  gcry_md_hash_buffer(chosenHashFunction, e_str, buffer2, MAX_ZZ_BYTES * 2);
  ZZFromBytes(e, e_str, gcry_md_get_algo_dlen(chosenHashFunction));
  if(MAX_ZZ_BYTES < gcry_md_get_algo_dlen(chosenHashFunction)) trunc(e, e, p_bits);
  unsigned char* tmpX_str = (unsigned char *)malloc(sizeof *tmpX_str * MAX_ZZ_BYTES);
  unsigned char* tmpY_str = (unsigned char *)malloc(sizeof *tmpY_str * MAX_ZZ_BYTES);
  for(int i = 0; i < MAX_ZZ_BYTES; i++) {
    tmpX_str[i] = (unsigned char)buffer2[i];
    tmpY_str[i] = (unsigned char)buffer2[i + MAX_ZZ_BYTES];
  }
  ZZFromBytes(tmpX, tmpX_str, MAX_ZZ_BYTES);
  ZZFromBytes(tmpY, tmpY_str, MAX_ZZ_BYTES);
  PU = new ECpoint(tmpX, tmpY);
  QU = theCurve->addPoints(theCurve->mulPoint(PU, e), QCA);
  AddMod(dU, r, MulMod(e, kU, n), n);
  std::cout << "Received response (r, CertU), where r = " << r << "\n";
  std::cout << "Calculated e=Hn(CertU), e = " << e << "\n";
  std::cout << "Decoded PU value from CertU, PU = ";
  PU->printlnPoint();
  std::cout << "My public key is QU = ";
  QU->printlnPoint();


  return 0;
}
