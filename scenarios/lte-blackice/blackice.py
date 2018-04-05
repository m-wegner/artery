import storyboard


def createStories(board):
    topleft = storyboard.Coord(400, 400)
    topright = storyboard.Coord(600, 400)
    bottomright = storyboard.Coord(600, 600)
    bottomleft = storyboard.Coord(400, 600)

    region = storyboard.PolygonCondition([topleft, topright, bottomright, bottomleft])
    fast = storyboard.SpeedConditionGreater(16.67)
    blackice = storyboard.AndCondition(region, fast)
    story = storyboard.Story(blackice, [storyboard.SignalEffect("traction loss")])
    board.registerStory(story)
