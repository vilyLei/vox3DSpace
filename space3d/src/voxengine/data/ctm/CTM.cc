#include <cmath>
#include "CTM.h"
namespace voxengine
{
namespace data
{
namespace ctm
{
CTM::CTM(std::shared_ptr<FileHeader> header) noexcept
{
}
CTM::~CTM() noexcept
{
}
std::shared_ptr<InterleavedStream> CTM::InterleavedStream(std::shared_ptr<voxengine::data::stream::DataView> data, Int32 count)
{
    return std::make_shared<voxengine::data::ctm::InterleavedStream>(data, count);
}

void CTM::restoreIndices(std::shared_ptr<voxengine::data::stream::Int32Array> pindices, Int32 len)
{
    auto& indices = (*pindices);
    Int32 i       = 3;
    if (len > 0)
    {
        indices[2] += indices[0];
        indices[1] += indices[0];
    }
    for (; i < len; i += 3)
    {
        indices[i] += indices[i - 3];

        if (indices[i] == indices[i - 3])
        {
            indices[i + 1] += indices[i - 2];
        }
        else
        {
            indices[i + 1] += indices[i];
        }

        indices[i + 2] += indices[i];
    }
}
std::shared_ptr<voxengine::data::stream::Float32Array> CTM::calcSmoothNormals(std::shared_ptr<voxengine::data::stream::Uint32Array> pindices, std::shared_ptr<voxengine::data::stream::Float32Array> pvertices)
{
    auto  psmooth = std::make_shared<voxengine::data::stream::Float32Array>(pvertices->getLength());
    auto& smooth  = (*psmooth);
    Int32 indx,
        indy,
        indz,
        i,
        k;

    double nx,
        ny,
        nz,
        v1x,
        v1y,
        v1z,
        v2x,
        v2y,
        v2z,
        len;

    auto& indices    = (*pindices);
    auto& vertices   = (*pvertices);
    auto  indicesLen = indices.getLength();
    for (i = 0, k = indicesLen; i < k;)
    {
        indx = indices[i++] * 3;
        indy = indices[i++] * 3;
        indz = indices[i++] * 3;

        v1x = vertices[indy] - vertices[indx];
        v2x = vertices[indz] - vertices[indx];
        v1y = vertices[indy + 1] - vertices[indx + 1];
        v2y = vertices[indz + 1] - vertices[indx + 1];
        v1z = vertices[indy + 2] - vertices[indx + 2];
        v2z = vertices[indz + 2] - vertices[indx + 2];

        nx = v1y * v2z - v1z * v2y;
        ny = v1z * v2x - v1x * v2z;
        nz = v1x * v2y - v1y * v2x;

        len = std::sqrt(nx * nx + ny * ny + nz * nz);
        if (len > 1e-10)
        {
            nx /= len;
            ny /= len;
            nz /= len;
        }

        smooth[indx] += nx;
        smooth[indx + 1] += ny;
        smooth[indx + 2] += nz;
        smooth[indy] += nx;
        smooth[indy + 1] += ny;
        smooth[indy + 2] += nz;
        smooth[indz] += nx;
        smooth[indz + 1] += ny;
        smooth[indz + 2] += nz;
    }
    auto smoothLen = smooth.getLength();
    for (i = 0, k = smoothLen; i < k; i += 3)
    {
        len = std::sqrt(smooth[i] * smooth[i] + smooth[i + 1] * smooth[i + 1] + smooth[i + 2] * smooth[i + 2]);

        if (len > 1e-10)
        {
            smooth[i] /= len;
            smooth[i + 1] /= len;
            smooth[i + 2] /= len;
        }
    }

    return psmooth;
}

void CTM::restoreVertices(std::shared_ptr<voxengine::data::stream::Float32Array> pvertices, std::shared_ptr<FileMG2Header> pgrid, std::shared_ptr<voxengine::data::stream::Uint32Array> pgridIndices, Float32 precision)
{
    auto& vertices    = (*pvertices);
    auto& grid        = (*pgrid);
    auto& gridIndices = (*pgridIndices);
    Int32 gridIdx,
        delta,
        x,
        y,
        z,
        ydiv           = grid.divx,
        zdiv           = ydiv * grid.divy,
        prevGridIdx    = 0x7fffffff,
        prevDelta      = 0,
        i              = 0,
        j              = 0,
        len            = gridIndices.getLength();
    auto  pintVertices = std::make_shared<voxengine::data::stream::Uint32Array>(vertices.getBuffer(), vertices.getByteOffset(), vertices.getLength());
    auto& intVertices  = (*pintVertices);
    for (; i < len; j += 3)
    {
        x = gridIdx = gridIndices[i++];

        z = ~~(x / zdiv);
        x -= ~~(z * zdiv);
        y = ~~(x / ydiv);
        x -= ~~(y * ydiv);

        delta = intVertices[j];
        if (gridIdx == prevGridIdx)
        {
            delta += prevDelta;
        }

        vertices[j]     = grid.lowerBoundx + x * grid.sizex + precision * delta;
        vertices[j + 1] = grid.lowerBoundy + y * grid.sizey + precision * intVertices[j + 1];
        vertices[j + 2] = grid.lowerBoundz + z * grid.sizez + precision * intVertices[j + 2];

        prevGridIdx = gridIdx;
        prevDelta   = delta;
    }
}


void CTM::restoreGridIndices(std::shared_ptr<voxengine::data::stream::Uint32Array> pgridIndices, Int32 len)
{
    auto& gridIndices = (*pgridIndices);
    auto  i           = 1;
    for (; i < len; ++i)
    {
        gridIndices[i] += gridIndices[i - 1];
    }
}


void CTM::restoreNormals(std::shared_ptr<voxengine::data::stream::Float32Array> pnormals,
                         std::shared_ptr<voxengine::data::stream::Float32Array> psmooth,
                         Float32                                                precision)
{
    auto& normals = (*pnormals);
    auto& smooth  = (*psmooth);
    Int32 i       = 0,
          k       = normals.getLength(),
          phi;
    double ro,
        theta,
        sinPhi,
        nx,
        ny,
        nz,
        by,
        bz,
        len,
        dphi,
        PI_DIV_2 = 3.141592653589793238462643 * 0.5;


    //std::cout << "      CTM::restoreNormals(), precision: " << precision << std::endl;
    auto  pintNormals = std::make_shared<voxengine::data::stream::Uint32Array>(normals.getBuffer(), normals.getByteOffset(), normals.getLength());
    auto& intNormals  = (*pintNormals);
    auto  dprecision  = static_cast<double>(precision);
    //std::cout << "      CTM::restoreNormals(), dprecision: " << dprecision << std::endl;
    for (; i < k; i += 3)
    {
        ro  = intNormals[i] * dprecision;
        phi = intNormals[i + 1];

        if (phi == 0)
        {
            normals[i]     = smooth[i] * ro;
            normals[i + 1] = smooth[i + 1] * ro;
            normals[i + 2] = smooth[i + 2] * ro;
        }
        else
        {
            if (phi <= 4)
            {
                theta = static_cast<double>(intNormals[i + 2] - 2) * PI_DIV_2;
            }
            else
            {
                theta = (static_cast<double>(intNormals[i + 2] * 4) / phi - 2.0) * PI_DIV_2;
            }

            dphi   = phi * dprecision * PI_DIV_2;
            sinPhi = ro * std::sin(dphi);

            nx = sinPhi * std::cos(theta);
            ny = sinPhi * std::sin(theta);
            nz = ro * std::cos(dphi);

            bz = smooth[i + 1];
            by = smooth[i] - smooth[i + 2];

            len = std::sqrt(2 * bz * bz + by * by);
            if (len > 1e-20)
            {
                by /= len;
                bz /= len;
            }

            normals[i]     = static_cast<Float32>(smooth[i] * nz + (smooth[i + 1] * bz - smooth[i + 2] * by) * ny - bz * nx);
            normals[i + 1] = static_cast<Float32>(smooth[i + 1] * nz - (smooth[i + 2] + smooth[i]) * bz * ny + by * nx);
            normals[i + 2] = static_cast<Float32>(smooth[i + 2] * nz + (smooth[i] * by + smooth[i + 1] * bz) * ny + bz * nx);
        }
    }
}

void CTM::restoreMap(std::shared_ptr<voxengine::data::stream::DataView> pmap, Int32 count, Float32 precision)
{
    auto& map = (*pmap);
    Int32 delta,
        value,
        i = 0,
        j,
        len = map.getLength();

    auto  pintMap = std::make_shared<voxengine::data::stream::Uint32Array>(map.getBuffer(), map.getByteOffset(), map.getLength());
    auto& intMap  = (*pintMap);
    auto  pf32Map = std::make_shared<voxengine::data::stream::Float32Array>(map.getBuffer(), map.getByteOffset(), map.getLength());
    auto& f32Map  = (*pf32Map);
    for (; i < count; ++i)
    {
        delta = 0;

        for (j = i; j < len; j += count)
        {
            value = intMap[j];

            delta += value & 1 ? -((value + 1) >> 1) : value >> 1;

            //map[j] = delta * precision;
            f32Map[j] = static_cast<Float32>(delta * precision);
        }
    }
}

} // namespace ctm
} // namespace data
} // namespace voxengine
