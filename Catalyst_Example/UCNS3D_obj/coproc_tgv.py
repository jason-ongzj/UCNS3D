
#--------------------------------------------------------------

# Global timestep output options
timeStepToStartOutputAt=0
forceOutputAtFirstCall=False

# Global screenshot output options
imageFileNamePadding=0
rescale_lookuptable=True

# Whether or not to request specific arrays from the adaptor.
requestSpecificArrays=False

# a root directory under which all Catalyst output goes
rootDirectory=''

# makes a cinema D index table
make_cinema_table=False

#--------------------------------------------------------------
# Code generated from cpstate.py to create the CoProcessor.
# paraview version 5.8.0-636-gfeb3424073
#--------------------------------------------------------------

from paraview.simple import *
from paraview import coprocessing

# ----------------------- CoProcessor definition -----------------------

def CreateCoProcessor():
  def _CreatePipeline(coprocessor, datadescription):
    class Pipeline:
      # state file generated using paraview version 5.8.0-636-gfeb3424073

      # ----------------------------------------------------------------
      # setup views used in the visualization
      # ----------------------------------------------------------------

      # generated using paraview version 5.8.0-636-gfeb3424073
      #
      # To ensure correct image size when batch processing, please search 
      # for and uncomment the line `# renderView*.ViewSize = [*,*]`

      #### disable automatic camera reset on 'Show'
      paraview.simple._DisableFirstRenderCameraReset()

      # Create a new 'Render View'
      renderView1 = CreateView('RenderView')
      renderView1.ViewSize = [900, 522]
      renderView1.AxesGrid = 'GridAxes3DActor'
      renderView1.CenterOfRotation = [3.1415927410125732, 3.1415927410125732, 3.1415927410125732]
      renderView1.StereoType = 'Crystal Eyes'
      renderView1.CameraPosition = [-6.962290992619889, 13.00713402789492, 18.716862694358497]
      renderView1.CameraFocalPoint = [3.141592741012574, 3.1415927410125715, 3.1415927410125755]
      renderView1.CameraViewUp = [0.21276442893019504, 0.8819412362199431, -0.4206078382969658]
      renderView1.CameraFocalDisk = 1.0
      renderView1.CameraParallelScale = 5.44139824412335

      # register the view with coprocessor
      # and provide it with information such as the filename to use,
      # how frequently to write the images, etc.
      coprocessor.RegisterView(renderView1,
          filename='RenderView1_%t.png', freq=10, fittoscreen=0, magnification=1, width=900, height=522, cinema={}, compression=5)
      renderView1.ViewTime = datadescription.GetTime()

      # Create a new 'Render View'
      renderView2 = CreateView('RenderView')
      renderView2.AxesGrid = 'GridAxes3DActor'
      renderView2.CenterOfRotation = [3.1415927410125732, 3.1415927410125732, 3.1415927410125732]
      renderView2.StereoType = 'Crystal Eyes'
      renderView2.CameraFocalDisk = 1.0
      renderView2.CameraParallelScale = 5.44139824412335
      # uncomment following to set a specific view size
      # renderView2.ViewSize = [400, 400]

      SetActiveView(None)

      # ----------------------------------------------------------------
      # setup view layouts
      # ----------------------------------------------------------------

      # create new layout object 'Layout #1'
      layout1 = CreateLayout(name='Layout #1')
      layout1.AssignView(0, renderView1)

      # ----------------------------------------------------------------
      # restore active view
      SetActiveView(renderView1)
      # ----------------------------------------------------------------

      # ----------------------------------------------------------------
      # setup the data processing pipelines
      # ----------------------------------------------------------------

      # create a new 'PVTrivialProducer'
      # create a producer from a simulation input
      input = coprocessor.CreateProducer(datadescription, 'input')

      # ----------------------------------------------------------------
      # setup the visualization in view 'renderView1'
      # ----------------------------------------------------------------

      # show data from input
      inputDisplay = Show(input, renderView1, 'UnstructuredGridRepresentation')

      # get color transfer function/color map for 'RU'
      rULUT = GetColorTransferFunction('RU')
      rULUT.AutomaticRescaleRangeMode = 'Clamp and update every timestep'
      rULUT.RGBPoints = [-0.9330251951608441, 0.231373, 0.298039, 0.752941, 0.0022094683390114778, 0.865003, 0.865003, 0.865003, 0.9374441318388671, 0.705882, 0.0156863, 0.14902]
      rULUT.ScalarRangeInitialized = 1.0

      # get opacity transfer function/opacity map for 'RU'
      rUPWF = GetOpacityTransferFunction('RU')
      rUPWF.Points = [-0.9330251951608441, 0.0, 0.5, 0.0, 0.9374441318388671, 1.0, 0.5, 0.0]
      rUPWF.ScalarRangeInitialized = 1

      # trace defaults for the display properties.
      inputDisplay.Representation = 'Surface'
      inputDisplay.ColorArrayName = ['CELLS', 'RU']
      inputDisplay.LookupTable = rULUT
      inputDisplay.OSPRayScaleFunction = 'PiecewiseFunction'
      inputDisplay.SelectOrientationVectors = 'None'
      inputDisplay.ScaleFactor = 0.6283185482025146
      inputDisplay.SelectScaleArray = 'None'
      inputDisplay.GlyphType = 'Arrow'
      inputDisplay.GlyphTableIndexArray = 'None'
      inputDisplay.GaussianRadius = 0.031415927410125735
      inputDisplay.SetScaleArray = [None, '']
      inputDisplay.ScaleTransferFunction = 'PiecewiseFunction'
      inputDisplay.OpacityArray = [None, '']
      inputDisplay.OpacityTransferFunction = 'PiecewiseFunction'
      inputDisplay.DataAxesGrid = 'GridAxesRepresentation'
      inputDisplay.PolarAxes = 'PolarAxesRepresentation'
      inputDisplay.ScalarOpacityFunction = rUPWF
      inputDisplay.ScalarOpacityUnitDistance = 0.5398550810562074

      # setup the color legend parameters for each legend in this view

      # get color legend/bar for rULUT in view renderView1
      rULUTColorBar = GetScalarBar(rULUT, renderView1)
      rULUTColorBar.Title = 'RU'
      rULUTColorBar.ComponentTitle = ''

      # set color bar visibility
      rULUTColorBar.Visibility = 1

      # show color legend
      inputDisplay.SetScalarBarVisibility(renderView1, True)

      # ----------------------------------------------------------------
      # setup the visualization in view 'renderView2'
      # ----------------------------------------------------------------

      # show data from input
      inputDisplay_1 = Show(input, renderView2, 'UnstructuredGridRepresentation')

      # trace defaults for the display properties.
      inputDisplay_1.Representation = 'Surface'
      inputDisplay_1.ColorArrayName = [None, '']
      inputDisplay_1.OSPRayScaleFunction = 'PiecewiseFunction'
      inputDisplay_1.SelectOrientationVectors = 'None'
      inputDisplay_1.ScaleFactor = 0.6283185482025146
      inputDisplay_1.SelectScaleArray = 'None'
      inputDisplay_1.GlyphType = 'Arrow'
      inputDisplay_1.GlyphTableIndexArray = 'None'
      inputDisplay_1.GaussianRadius = 0.031415927410125735
      inputDisplay_1.SetScaleArray = [None, '']
      inputDisplay_1.ScaleTransferFunction = 'PiecewiseFunction'
      inputDisplay_1.OpacityArray = [None, '']
      inputDisplay_1.OpacityTransferFunction = 'PiecewiseFunction'
      inputDisplay_1.DataAxesGrid = 'GridAxesRepresentation'
      inputDisplay_1.PolarAxes = 'PolarAxesRepresentation'
      inputDisplay_1.ScalarOpacityUnitDistance = 0.5398550810562074

      # ----------------------------------------------------------------
      # setup color maps and opacity mapes used in the visualization
      # note: the Get..() functions create a new object, if needed
      # ----------------------------------------------------------------

      # ----------------------------------------------------------------
      # finally, restore active source
      SetActiveSource(input)
      # ----------------------------------------------------------------
    return Pipeline()

  class CoProcessor(coprocessing.CoProcessor):
    def CreatePipeline(self, datadescription):
      self.Pipeline = _CreatePipeline(self, datadescription)

  coprocessor = CoProcessor()
  # these are the frequencies at which the coprocessor updates.
  freqs = {'input': [10]}
  coprocessor.SetUpdateFrequencies(freqs)
  if requestSpecificArrays:
    arrays = [['E', 1], ['RU', 1], ['RV', 1]]
    coprocessor.SetRequestedArrays('input', arrays)
  coprocessor.SetInitialOutputOptions(timeStepToStartOutputAt,forceOutputAtFirstCall)

  if rootDirectory:
      coprocessor.SetRootDirectory(rootDirectory)

  if make_cinema_table:
      coprocessor.EnableCinemaDTable()

  return coprocessor


#--------------------------------------------------------------
# Global variable that will hold the pipeline for each timestep
# Creating the CoProcessor object, doesn't actually create the ParaView pipeline.
# It will be automatically setup when coprocessor.UpdateProducers() is called the
# first time.
coprocessor = CreateCoProcessor()

#--------------------------------------------------------------
# Enable Live-Visualizaton with ParaView and the update frequency
coprocessor.EnableLiveVisualization(True)

# ---------------------- Data Selection method ----------------------

def RequestDataDescription(datadescription):
    "Callback to populate the request for current timestep"
    global coprocessor

    # setup requests for all inputs based on the requirements of the
    # pipeline.
    coprocessor.LoadRequestedData(datadescription)

# ------------------------ Processing method ------------------------

def DoCoProcessing(datadescription):
    "Callback to do co-processing for current timestep"
    global coprocessor

    # Update the coprocessor by providing it the newly generated simulation data.
    # If the pipeline hasn't been setup yet, this will setup the pipeline.
    coprocessor.UpdateProducers(datadescription)

    # Write output data, if appropriate.
    coprocessor.WriteData(datadescription);

    # Write image capture (Last arg: rescale lookup table), if appropriate.
    coprocessor.WriteImages(datadescription, rescale_lookuptable=True,
        image_quality=0, padding_amount=imageFileNamePadding)

    # Live Visualization, if enabled.
    coprocessor.DoLiveVisualization(datadescription, "localhost", 22222)
