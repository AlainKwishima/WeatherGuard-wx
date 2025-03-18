#include <scwx/wsr88d/rpg/linked_contour_vector_packet.hpp>
#include <scwx/util/logger.hpp>

#include <istream>
#include <string>

namespace scwx
{
namespace wsr88d
{
namespace rpg
{

static const std::string logPrefix_ =
   "scwx::wsr88d::rpg::linked_contour_vector_packet";
static const auto logger_ = util::Logger::Create(logPrefix_);

class LinkedContourVectorPacketImpl
{
public:
   explicit LinkedContourVectorPacketImpl() :
       packetCode_ {0},
       initialPointIndicator_ {0},
       lengthOfVectors_ {0},
       startI_ {0},
       startJ_ {0},
       endI_ {},
       endJ_ {}
   {
   }
   ~LinkedContourVectorPacketImpl() = default;

   uint16_t packetCode_;
   uint16_t initialPointIndicator_;
   uint16_t lengthOfVectors_;

   int16_t              startI_;
   int16_t              startJ_;
   std::vector<int16_t> endI_;
   std::vector<int16_t> endJ_;
};

LinkedContourVectorPacket::LinkedContourVectorPacket() :
    p(std::make_unique<LinkedContourVectorPacketImpl>())
{
}
LinkedContourVectorPacket::~LinkedContourVectorPacket() = default;

LinkedContourVectorPacket::LinkedContourVectorPacket(
   LinkedContourVectorPacket&&) noexcept                      = default;
LinkedContourVectorPacket& LinkedContourVectorPacket::operator=(
   LinkedContourVectorPacket&&) noexcept = default;

uint16_t LinkedContourVectorPacket::packet_code() const
{
   return p->packetCode_;
}

uint16_t LinkedContourVectorPacket::initial_point_indicator() const
{
   return p->initialPointIndicator_;
}

uint16_t LinkedContourVectorPacket::length_of_vectors() const
{
   return p->lengthOfVectors_;
}

size_t LinkedContourVectorPacket::data_size() const
{
   return p->lengthOfVectors_ + 10u;
}

bool LinkedContourVectorPacket::Parse(std::istream& is)
{
   bool blockValid = true;

   std::streampos isBegin = is.tellg();

   is.read(reinterpret_cast<char*>(&p->packetCode_), 2);
   is.read(reinterpret_cast<char*>(&p->initialPointIndicator_), 2);
   is.read(reinterpret_cast<char*>(&p->startI_), 2);
   is.read(reinterpret_cast<char*>(&p->startJ_), 2);
   is.read(reinterpret_cast<char*>(&p->lengthOfVectors_), 2);

   p->packetCode_            = ntohs(p->packetCode_);
   p->initialPointIndicator_ = ntohs(p->initialPointIndicator_);
   p->startI_                = ntohs(p->startI_);
   p->startJ_                = ntohs(p->startJ_);
   p->lengthOfVectors_       = ntohs(p->lengthOfVectors_);

   int vectorSize = static_cast<int>(p->lengthOfVectors_);

   if (is.eof())
   {
      logger_->debug("Reached end of file");
      blockValid = false;
   }
   else
   {
      if (p->packetCode_ != 0x0E03)
      {
         logger_->warn("Invalid packet code: {}", p->packetCode_);
         blockValid = false;
      }
      if (p->initialPointIndicator_ != 0x8000)
      {
         logger_->warn("Invalid initial point indicator: {}",
                       p->initialPointIndicator_);
         blockValid = false;
      }
   }

   if (blockValid)
   {
      // The number of vectors is equal to the size divided by the number of
      // bytes in a vector coordinate
      int vectorCount = vectorSize / 4;

      p->endI_.resize(vectorCount);
      p->endJ_.resize(vectorCount);

      for (int v = 0; v < vectorCount && !is.eof(); v++)
      {
         is.read(reinterpret_cast<char*>(&p->endI_[v]), 2);
         is.read(reinterpret_cast<char*>(&p->endJ_[v]), 2);

         p->endI_[v] = ntohs(p->endI_[v]);
         p->endJ_[v] = ntohs(p->endJ_[v]);
      }
   }

   std::streampos isEnd     = is.tellg();
   std::streamoff bytesRead = isEnd - isBegin;

   if (!ValidateMessage(is, bytesRead))
   {
      blockValid = false;
   }

   return blockValid;
}

std::shared_ptr<LinkedContourVectorPacket>
LinkedContourVectorPacket::Create(std::istream& is)
{
   std::shared_ptr<LinkedContourVectorPacket> packet =
      std::make_shared<LinkedContourVectorPacket>();

   if (!packet->Parse(is))
   {
      packet.reset();
   }

   return packet;
}

} // namespace rpg
} // namespace wsr88d
} // namespace scwx
