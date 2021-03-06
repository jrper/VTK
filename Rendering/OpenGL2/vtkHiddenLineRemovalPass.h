/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkHiddenLineRemovalPass.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// .NAME vtkHiddenLineRemovalPass - RenderPass for HLR.
//
// .SECTION Description
// This render pass renders wireframe polydata such that only the front
// wireframe surface is drawn.

#ifndef vtkHiddenLineRemovalPass_h
#define vtkHiddenLineRemovalPass_h

#include "vtkRenderingOpenGL2Module.h" // For export macro
#include "vtkOpenGLRenderPass.h"

#include <vector> // For std::vector!

class vtkProp;
class vtkViewport;

class VTKRENDERINGOPENGL2_EXPORT vtkHiddenLineRemovalPass :
    public vtkOpenGLRenderPass
{
public:
  static vtkHiddenLineRemovalPass* New();
  vtkTypeMacro(vtkHiddenLineRemovalPass, vtkOpenGLRenderPass)
  virtual void PrintSelf(ostream &os, vtkIndent indent);

  virtual void Render(const vtkRenderState *s);

  // Description:
  // Returns true if any of the nProps in propArray are rendered as wireframe.
  static bool WireframePropsExist(vtkProp **propArray, int nProps);

protected:
  vtkHiddenLineRemovalPass();
  ~vtkHiddenLineRemovalPass();

  void SetRepresentation(std::vector<vtkProp*> &props, int repr);
  int RenderProps(std::vector<vtkProp*> &props, vtkViewport *vp);

private:
  vtkHiddenLineRemovalPass(const vtkHiddenLineRemovalPass&) VTK_DELETE_FUNCTION;
  void operator=(const vtkHiddenLineRemovalPass&) VTK_DELETE_FUNCTION;
};

#endif // vtkHiddenLineRemovalPass_h
