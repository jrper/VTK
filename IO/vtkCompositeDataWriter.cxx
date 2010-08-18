/*=========================================================================

  Program:   ParaView
  Module:    $RCSfile$

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkCompositeDataWriter.h"

#include "vtkHierarchicalBoxDataSet.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkMultiPieceDataSet.h"
#include "vtkObjectFactory.h"
#include "vtkGenericDataObjectWriter.h"
#include "vtkInformation.h"

vtkStandardNewMacro(vtkCompositeDataWriter);
//----------------------------------------------------------------------------
vtkCompositeDataWriter::vtkCompositeDataWriter()
{
}

//----------------------------------------------------------------------------
vtkCompositeDataWriter::~vtkCompositeDataWriter()
{
}

//----------------------------------------------------------------------------
vtkCompositeDataSet* vtkCompositeDataWriter::GetInput()
{
  return this->GetInput(0);
}

//----------------------------------------------------------------------------
vtkCompositeDataSet* vtkCompositeDataWriter::GetInput(int port)
{
  return vtkCompositeDataSet::SafeDownCast(this->GetInputDataObject(0, 0));
}

//----------------------------------------------------------------------------
int vtkCompositeDataWriter::FillInputPortInformation(int, vtkInformation *info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkCompositeDataSet");
  return 1;
}

//----------------------------------------------------------------------------
void vtkCompositeDataWriter::WriteData()
{
  ostream *fp;
  vtkCompositeDataSet *input = this->GetInput();

  vtkDebugMacro(<<"Writing vtk composite data...");
  if ( !(fp=this->OpenVTKFile()) || !this->WriteHeader(fp) )
    {
    if (fp)
      {
      if(this->FileName)
        {
        vtkErrorMacro(
          "Ran out of disk space; deleting file: " << this->FileName);
        this->CloseVTKFile(fp);
        unlink(this->FileName);
        }
      else
        {
        this->CloseVTKFile(fp);
        vtkErrorMacro("Could not read memory header. ");
        }
      }
    return;
    }

  vtkMultiBlockDataSet* mb = vtkMultiBlockDataSet::SafeDownCast(input);
  vtkHierarchicalBoxDataSet* hb =
    vtkHierarchicalBoxDataSet::SafeDownCast(input);
  vtkMultiPieceDataSet* mp = vtkMultiPieceDataSet::SafeDownCast(input);
  if (mb)
    {
    *fp << "DATASET MULTIBLOCK\n";
    this->WriteCompositeData(fp, mb);
    }
  else if (hb)
    {
    *fp << "DATASET HIERARCHICAL BOX\n";
    this->WriteCompositeData(fp, hb);
    }
  else if (mp)
    {
    *fp << "DATASET MULTIPIECE\n";
    this->WriteCompositeData(fp, mp);
    }
  else
    {
    vtkErrorMacro("Unsupported input type: " << input->GetClassName());
    this->CloseVTKFile(fp);
    return;
    }

  this->CloseVTKFile(fp);
}

//----------------------------------------------------------------------------
bool vtkCompositeDataWriter::WriteCompositeData(ostream* fp,
  vtkMultiBlockDataSet* mb)
{
  *fp << "CHILDREN " << mb->GetNumberOfBlocks() << "\n";
  for (unsigned int cc=0; cc < mb->GetNumberOfBlocks(); cc++)
    {
    vtkDataObject* child = mb->GetBlock(cc);
    *fp << "CHILD " << (child? child->GetDataObjectType() : -1) << "\n";
    if (child)
      {
      if (!this->WriteBlock(fp, child))
        {
        return false;
        }
      }
    *fp << "ENDCHILD\n";
    }

  return true;
}

//----------------------------------------------------------------------------
bool vtkCompositeDataWriter::WriteCompositeData(ostream* fp,
  vtkMultiPieceDataSet* mp)
{
  return false;
}

//----------------------------------------------------------------------------
bool vtkCompositeDataWriter::WriteCompositeData(ostream* fp,
  vtkHierarchicalBoxDataSet* hb)
{
  return false;
}

//----------------------------------------------------------------------------
bool vtkCompositeDataWriter::WriteBlock(ostream* fp, vtkDataObject* block)
{
  bool success = false;
  vtkGenericDataObjectWriter* writer = vtkGenericDataObjectWriter::New();
  writer->WriteToOutputStringOn();
  writer->SetFileType(this->FileType);
  writer->SetInput(block);
  if (writer->Write())
    {
    fp->write(
      reinterpret_cast<const char*>(writer->GetBinaryOutputString()),
      writer->GetOutputStringLength());
    success = true;
    }
  writer->Delete();
  return success;
}

//----------------------------------------------------------------------------
void vtkCompositeDataWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
