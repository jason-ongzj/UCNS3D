#ifndef VTKCPVTKPIPELINE_H
#define VTKCPVTKPIPELINE_H

#include <string>
#include <vtkCPPipeline.h>
#include <vtkMultiProcessController.h>

class vtkCPDataDescription;

class vtkCPVTKPipeline : public vtkCPPipeline{

public:
  static vtkCPVTKPipeline* New();
  vtkTypeMacro(vtkCPVTKPipeline, vtkCPPipeline);

  virtual void Initialize(int outputFrequency);

  virtual int RequestDataDescription(vtkCPDataDescription* dataDescription);

  virtual int CoProcess(vtkCPDataDescription* dataDescription);

protected:
  vtkCPVTKPipeline();
  virtual ~vtkCPVTKPipeline();

private:
  vtkCPVTKPipeline(const vtkCPVTKPipeline&) = delete;
  void operator=(const vtkCPVTKPipeline&) = delete;

  int OutputFrequency;
};
#endif
