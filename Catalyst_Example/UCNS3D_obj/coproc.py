
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
# paraview version 5.8.0
#--------------------------------------------------------------

from paraview.simple import *
from paraview import coprocessing

# ----------------------- CoProcessor definition -----------------------

def CreateCoProcessor():
  def _CreatePipeline(coprocessor, datadescription):
    class Pipeline:
      # state file generated using paraview version 5.8.0

      # ----------------------------------------------------------------
      # setup views used in the visualization
      # ----------------------------------------------------------------

      # trace generated using paraview version 5.8.0
      #
      # To ensure correct image size when batch processing, please search 
      # for and uncomment the line `# renderView*.ViewSize = [*,*]`

      #### disable automatic camera reset on 'Show'
      paraview.simple._DisableFirstRenderCameraReset()

      # Create a new 'Render View'
      renderView4 = CreateView('RenderView')
      renderView4.ViewSize = [1256, 799]
      renderView4.AxesGrid = 'GridAxes3DActor'
      renderView4.CenterOfRotation = [50.30695152282715, 0.0, 9.869604110717773]
      renderView4.StereoType = 'Crystal Eyes'
      renderView4.CameraPosition = [35.82379571216059, -3.9871568578962098, 329.7784155802543]
      renderView4.CameraFocalPoint = [35.82379571216059, -3.9871568578962098, 9.869604110717773]
      renderView4.CameraFocalDisk = 1.0
      renderView4.CameraParallelScale = 121.2252737541929

      # register the view with coprocessor
      # and provide it with information such as the filename to use,
      # how frequently to write the images, etc.
      coprocessor.RegisterView(renderView4,
          filename='RenderView4_%t.png', freq=20, fittoscreen=0, magnification=1, width=1477, height=799, cinema={}, compression=5)
      renderView4.ViewTime = datadescription.GetTime()

      SetActiveView(None)

      # ----------------------------------------------------------------
      # setup view layouts
      # ----------------------------------------------------------------

      # create new layout object 'Layout #1'
      layout1 = CreateLayout(name='Layout #1')
      layout1.AssignView(0, renderView4)

      # ----------------------------------------------------------------
      # restore active view
      SetActiveView(renderView4)
      # ----------------------------------------------------------------

      # ----------------------------------------------------------------
      # setup the data processing pipelines
      # ----------------------------------------------------------------

      # create a new 'PVTrivialProducer'
      # extractinput = PVTrivialProducer()
      extractinput = coprocessor.CreateProducer(datadescription, 'input')

      # ----------------------------------------------------------------
      # setup the visualization in view 'renderView4'
      # ----------------------------------------------------------------

      # show data from extractinput
      extractinputDisplay = Show(extractinput, renderView4, 'UnstructuredGridRepresentation')

      # get color transfer function/color map for 'RU'
      rULUT = GetColorTransferFunction('RU')
      rULUT.RGBPoints = [-0.9946198892277626, 0.231373, 0.298039, 0.752941, 3.3306690738754696e-16, 0.865003, 0.865003, 0.865003, 0.9946198892277632, 0.705882, 0.0156863, 0.14902]
      rULUT.ScalarRangeInitialized = 1.0

      # get opacity transfer function/opacity map for 'RU'
      rUPWF = GetOpacityTransferFunction('RU')
      rUPWF.Points = [-0.9946198892277626, 0.0, 0.5, 0.0, 0.9946198892277632, 1.0, 0.5, 0.0]
      rUPWF.ScalarRangeInitialized = 1

      # trace defaults for the display properties.
      extractinputDisplay.Representation = 'Surface'
      extractinputDisplay.ColorArrayName = ['CELLS', 'RU']
      extractinputDisplay.LookupTable = rULUT
      extractinputDisplay.OSPRayScaleArray = 'E'
      extractinputDisplay.OSPRayScaleFunction = 'PiecewiseFunction'
      extractinputDisplay.SelectOrientationVectors = 'E'
      extractinputDisplay.ScaleFactor = 21.35453681945801
      extractinputDisplay.SelectScaleArray = 'E'
      extractinputDisplay.GlyphType = 'Arrow'
      extractinputDisplay.GlyphTableIndexArray = 'E'
      extractinputDisplay.GaussianRadius = 1.0677268409729004
      extractinputDisplay.SetScaleArray = ['POINTS', 'E']
      extractinputDisplay.ScaleTransferFunction = 'PiecewiseFunction'
      extractinputDisplay.OpacityArray = ['POINTS', 'E']
      extractinputDisplay.OpacityTransferFunction = 'PiecewiseFunction'
      extractinputDisplay.DataAxesGrid = 'GridAxesRepresentation'
      extractinputDisplay.PolarAxes = 'PolarAxesRepresentation'
      extractinputDisplay.ScalarOpacityFunction = rUPWF
      extractinputDisplay.ScalarOpacityUnitDistance = 2.6210131800440215

      # init the 'PiecewiseFunction' selected for 'ScaleTransferFunction'
      extractinputDisplay.ScaleTransferFunction.Points = [0.0, 0.0, 0.5, 0.0, 253.52381186434286, 1.0, 0.5, 0.0]

      # init the 'PiecewiseFunction' selected for 'OpacityTransferFunction'
      extractinputDisplay.OpacityTransferFunction.Points = [0.0, 0.0, 0.5, 0.0, 253.52381186434286, 1.0, 0.5, 0.0]

      # setup the color legend parameters for each legend in this view

      # get color legend/bar for rULUT in view renderView4
      rULUTColorBar = GetScalarBar(rULUT, renderView4)
      rULUTColorBar.WindowLocation = 'AnyLocation'
      rULUTColorBar.Position = [0.045485640938839986, 0.35294117647058826]
      rULUTColorBar.Title = 'RU'
      rULUTColorBar.ComponentTitle = ''
      rULUTColorBar.ScalarBarLength = 0.3300000000000001

      # set color bar visibility
      rULUTColorBar.Visibility = 1

      # show color legend
      extractinputDisplay.SetScalarBarVisibility(renderView4, True)

      # ----------------------------------------------------------------
      # setup color maps and opacity mapes used in the visualization
      # note: the Get..() functions create a new object, if needed
      # ----------------------------------------------------------------

      # ----------------------------------------------------------------
      # finally, restore active source
      SetActiveSource(extractinput)
      # ----------------------------------------------------------------
    return Pipeline()

  class CoProcessor(coprocessing.CoProcessor):
    def CreatePipeline(self, datadescription):
      self.Pipeline = _CreatePipeline(self, datadescription)

  coprocessor = CoProcessor()
  # these are the frequencies at which the coprocessor updates.
  freqs = {'input': [20]}
  coprocessor.SetUpdateFrequencies(freqs)
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
    coprocessor.WriteImages(datadescription, rescale_lookuptable=rescale_lookuptable,
        image_quality=0, padding_amount=imageFileNamePadding)

    # Live Visualization, if enabled.
    coprocessor.DoLiveVisualization(datadescription, "localhost", 22222)
